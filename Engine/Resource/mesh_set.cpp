#include "mesh_set.h"
#include <bit>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include <string>

namespace dingosdk::frostbite {
namespace {
// The resource: three layout words this reader knows, the skinned flag, the levels of detail
// (a table of pointers) and their count. A pointer counts from byte 16.
constexpr std::array<std::uint32_t, 4> known_header{208, 188, 384, 0};
constexpr std::size_t skinned_flag = 124, lod_count = 180, lod_table = 48, pointer_base = 16;
// A level of detail: its kind (1), its sections, the index format (33: 16-bit indices), the
// buffer sizes and the geometry chunk.
constexpr std::size_t lod_kind = 0, section_count = 8, section_table = 12, index_format = 84, index_bytes = 88,
    vertex_bytes = 92, geometry_chunk = 116;
constexpr std::uint32_t known_lod_kind = 1, sixteen_bit_indices = 33;
// A section, 384 bytes: its bone palette, primitive type (3: a triangle list), triangles, first
// index, where its vertices start in the vertex buffer and how many, and its vertex declaration.
constexpr std::size_t section_size = 384, palette = 16, palette_count = 24, primitive = 31, triangle_count = 32,
    first_index = 36, vertex_start = 40, vertex_count = 44, declaration = 112;
constexpr std::uint8_t triangle_list = 3;
// The declaration: 16 elements of {usage, format, offset, stream}, then {stride, classification}
// per stream, then the element and stream counts. Four bone influences a vertex, or eight with
// the second pair (the coarser levels of detail leave it out).
constexpr std::size_t stream_table = 64, element_total = 96, stream_total = 97;
enum Usage : std::uint8_t { position = 1, bones_low = 2, bones_high = 3, weights_low = 4, weights_high = 5,
    texcoord0 = 6, texcoord1 = 7 };
constexpr std::array<std::uint8_t, 8> usage_format{0, 3, 23, 23, 13, 13, 2, 2}; // float3, ushort4, ushort4, ubyte4n, ubyte4n, float2, float2
constexpr std::array<std::uint8_t, 8> usage_size{0, 12, 8, 8, 4, 4, 8, 8};
constexpr std::uint8_t packed_texcoord0 = 33, packed_texcoord1 = 34, half2_format = 6;

void require(bool valid, const char* message) {
    if (!valid) throw std::runtime_error(std::string("MeshSet: ") + message);
}
struct Reader {
    std::span<const std::byte> data;
    template <class T> T at(std::size_t offset) const {
        require(offset <= data.size() && sizeof(T) <= data.size() - offset, "it is truncated");
        T value{};
        std::memcpy(&value, data.data() + offset, sizeof(T));
        return value;
    }
    std::size_t pointer(std::size_t offset) const {
        const auto value = at<std::uint64_t>(offset);
        require(value && value <= data.size() - pointer_base, "a pointer leads outside it");
        return static_cast<std::size_t>(value) + pointer_base;
    }
};
struct Attribute {
    std::size_t base{}, stride{}, offset{};
    bool present{};
    std::uint8_t format{};
};

float half_to_float(std::uint16_t value) {
    const auto sign = static_cast<std::uint32_t>(value & 0x8000) << 16;
    auto exponent = static_cast<std::uint32_t>((value >> 10) & 0x1f);
    auto mantissa = static_cast<std::uint32_t>(value & 0x3ff);
    std::uint32_t bits{};
    if (exponent == 0) {
        if (mantissa) {
            exponent = 113;
            while ((mantissa & 0x400) == 0) {
                mantissa <<= 1;
                --exponent;
            }
            bits = sign | (exponent << 23) | ((mantissa & 0x3ff) << 13);
        } else {
            bits = sign;
        }
    } else if (exponent == 0x1f) {
        bits = sign | 0x7f800000 | (mantissa << 13);
    } else {
        bits = sign | ((exponent + 112) << 23) | (mantissa << 13);
    }
    return std::bit_cast<float>(bits);
}
}

MeshLod read_mesh_lod(std::span<const std::byte> resource, std::size_t lod) {
    const Reader r{resource};
    for (std::size_t i = 0; i < known_header.size(); ++i)
        require(r.at<std::uint32_t>(i * 4) == known_header[i], "its layout is not the one this reader knows");
    require(r.at<std::uint8_t>(skinned_flag) == 1, "it is not skinned");
    require(lod < r.at<std::uint16_t>(lod_count), "it has no such level of detail");
    const auto at = r.pointer(lod_table + lod * 8);
    require(r.at<std::uint32_t>(at + lod_kind) == known_lod_kind, "the level of detail is of another kind");
    require(r.at<std::uint32_t>(at + index_format) == sixteen_bit_indices, "its indices are not 16-bit");
    MeshLod result;
    result.chunk = r.at<Guid>(at + geometry_chunk);
    result.vertex_bytes = r.at<std::uint32_t>(at + vertex_bytes);
    result.index_bytes = r.at<std::uint32_t>(at + index_bytes);
    require(result.chunk != Guid{} && result.vertex_bytes > 0 && result.index_bytes > 0, "the level of detail is empty");
    return result;
}

std::size_t mesh_lod_count(std::span<const std::byte> resource) {
    const Reader r{resource};
    for (std::size_t i = 0; i < known_header.size(); ++i)
        require(r.at<std::uint32_t>(i * 4) == known_header[i], "its layout is not the one this reader knows");
    return r.at<std::uint16_t>(lod_count);
}

MeshLod mesh_lod_geometry(std::span<const std::byte> resource, std::size_t lod) {
    require(lod < mesh_lod_count(resource), "it has no such level of detail");
    const Reader r{resource};
    const auto at = r.pointer(lod_table + lod * 8);
    require(r.at<std::uint32_t>(at + lod_kind) == known_lod_kind, "the level of detail is of another kind");
    MeshLod result;
    result.chunk = r.at<Guid>(at + geometry_chunk);
    result.vertex_bytes = r.at<std::uint32_t>(at + vertex_bytes);
    result.index_bytes = r.at<std::uint32_t>(at + index_bytes);
    require(result.chunk != Guid{} && result.vertex_bytes > 0, "the level of detail is empty");
    return result;
}

std::vector<std::array<float, 3>> read_mesh_positions(std::span<const std::byte> resource, std::size_t lod,
                                                      std::span<const std::byte> geometry) {
    const auto layout = mesh_lod_geometry(resource, lod);
    const Reader r{resource}, g{geometry};
    require(geometry.size() >= layout.vertex_bytes, "the geometry chunk is too short");
    const auto at = r.pointer(lod_table + lod * 8);
    const auto sections = r.at<std::uint32_t>(at + section_count);
    require(sections <= 4096, "it has too many sections");
    const auto table = r.pointer(at + section_table);
    std::vector<std::array<float, 3>> result;
    for (std::uint32_t section = 0; section < sections; ++section) {
        const auto s = table + section * section_size;
        const std::size_t count = r.at<std::uint32_t>(s + vertex_count);
        if (!count) continue;
        const auto d = s + declaration;
        const auto elements = r.at<std::uint8_t>(d + element_total), streams = r.at<std::uint8_t>(d + stream_total);
        require(elements <= 16 && streams > 0 && streams <= 16, "a vertex declaration is malformed");
        // Each stream's vertices follow the one before, from where the section starts.
        std::array<std::size_t, 16> bases{}, strides{};
        std::size_t end = r.at<std::uint32_t>(s + vertex_start);
        for (std::size_t stream = 0; stream < streams; ++stream) {
            strides[stream] = r.at<std::uint8_t>(d + stream_table + stream * 2);
            require(strides[stream] > 0, "a vertex stream has no size");
            bases[stream] = end;
            end += strides[stream] * count;
        }
        require(end <= layout.vertex_bytes, "a section's vertices run past the vertex buffer");
        bool found{};
        for (std::size_t element = 0; element < elements && !found; ++element) {
            if (r.at<std::uint8_t>(d + element * 4) != position) continue;
            const auto stream = r.at<std::uint8_t>(d + element * 4 + 3);
            const std::size_t offset = r.at<std::uint8_t>(d + element * 4 + 2);
            require(r.at<std::uint8_t>(d + element * 4 + 1) == usage_format[position] && stream < streams &&
                    offset + usage_size[position] <= strides[stream], "its positions have another format");
            for (std::size_t vertex = 0; vertex < count; ++vertex)
                result.push_back(g.at<std::array<float, 3>>(bases[stream] + vertex * strides[stream] + offset));
            found = true;
        }
        require(found, "a section has no positions");
    }
    return result;
}

SkinnedMesh read_skinned_mesh(std::span<const std::byte> resource, std::size_t lod, std::span<const std::byte> geometry) {
    const auto layout = read_mesh_lod(resource, lod);
    const Reader r{resource}, g{geometry};
    require(geometry.size() >= std::size_t{layout.vertex_bytes} + layout.index_bytes, "the geometry chunk is too short");
    const auto at = r.pointer(lod_table + lod * 8);
    const auto sections = r.at<std::uint32_t>(at + section_count);
    const auto table = r.pointer(at + section_table);
    SkinnedMesh result;
    result.has_uv0 = true;
    result.has_uv1 = true;
    for (std::uint32_t section = 0; section < sections; ++section) {
        const auto s = table + section * section_size;
        require(r.at<std::uint8_t>(s + primitive) == triangle_list, "a section is not a triangle list");
        const auto count = r.at<std::uint32_t>(s + vertex_count), triangles = r.at<std::uint32_t>(s + triangle_count);
        const auto bones = r.pointer(s + palette);
        const auto bone_count = r.at<std::uint16_t>(s + palette_count);
        const auto d = s + declaration;
        const auto elements = r.at<std::uint8_t>(d + element_total), streams = r.at<std::uint8_t>(d + stream_total);
        require(elements <= 16 && streams > 0 && streams <= 16, "a vertex declaration is malformed");
        std::array<std::size_t, 16> bases{}, strides{};
        std::size_t end = r.at<std::uint32_t>(s + vertex_start);
        for (std::size_t stream = 0; stream < streams; ++stream) {
            strides[stream] = r.at<std::uint8_t>(d + stream_table + stream * 2);
            require(strides[stream] > 0 && r.at<std::uint8_t>(d + stream_table + stream * 2 + 1) == 0,
                "a vertex stream is not per vertex");
            bases[stream] = end;
            end += strides[stream] * count;
        }
        require(end <= layout.vertex_bytes, "a section's vertices run past the vertex buffer");
        std::array<Attribute, 8> attributes{};
        Attribute source_id_color;
        for (std::size_t element = 0; element < elements; ++element) {
            const auto element_at = d + element * 4;
            const auto raw_usage = r.at<std::uint8_t>(element_at);
            const auto format = r.at<std::uint8_t>(element_at + 1);
            if (raw_usage == 30) {
                const auto stream = r.at<std::uint8_t>(element_at + 3);
                const std::size_t offset = r.at<std::uint8_t>(element_at + 2);
                require(format == 13 && stream < streams && offset + 4 <= strides[stream],
                    "its source-ID color has another format");
                source_id_color = {bases[stream], strides[stream], offset, true, format};
                continue;
            }
            const auto usage = raw_usage == packed_texcoord0 || raw_usage == 6 ? texcoord0 :
                raw_usage == packed_texcoord1 ? texcoord1 : raw_usage;
            if (usage < position || usage > texcoord1) continue;
            const auto stream = r.at<std::uint8_t>(element_at + 3);
            const std::size_t offset = r.at<std::uint8_t>(element_at + 2);
            const bool uv = usage == texcoord0 || usage == texcoord1;
            const bool supported_uv_format = uv &&
                (format == usage_format[usage] || (format == half2_format && raw_usage != 6));
            if (uv && !supported_uv_format) {
                if (usage == texcoord0) result.has_uv0 = false;
                else result.has_uv1 = false;
                continue;
            }
            const auto size = uv && format == half2_format ? 4 : usage_size[usage];
            require(stream < streams && offset + size <= strides[stream] &&
                    (uv || format == usage_format[usage]),
                "a skinning attribute has another format");
            attributes[usage] = {bases[stream], strides[stream], offset, true, format};
        }
        for (const auto usage : {position, bones_low, weights_low}) require(attributes[usage].present, "a section is not skinned");
        require(attributes[bones_high].present == attributes[weights_high].present, "a section's second influences are half there");
        if (!attributes[texcoord0].present) result.has_uv0 = false;
        if (!attributes[texcoord1].present) result.has_uv1 = false;
        if (source_id_color.present) result.has_source_id_color = true;
        const std::size_t influences = attributes[bones_high].present ? 8 : 4;
        const auto address = [&](std::uint8_t usage, std::size_t vertex) {
            const auto& a = attributes[usage];
            return a.base + vertex * a.stride + a.offset;
        };
        const auto first = static_cast<std::uint32_t>(result.vertices.size());
        for (std::size_t vertex = 0; vertex < count; ++vertex) {
            SkinnedVertex out;
            out.position = g.at<std::array<float, 3>>(address(position, vertex));
            if (attributes[texcoord0].present) {
                const auto uv_address = address(texcoord0, vertex);
                if (attributes[texcoord0].format == half2_format) {
                    out.uv0 = {half_to_float(g.at<std::uint16_t>(uv_address)),
                        half_to_float(g.at<std::uint16_t>(uv_address + 2))};
                } else {
                    out.uv0 = g.at<std::array<float, 2>>(uv_address);
                }
            }
            if (source_id_color.present)
                out.source_id_color = g.at<std::array<std::uint8_t, 4>>(
                    source_id_color.base + vertex * source_id_color.stride + source_id_color.offset);
            if (attributes[texcoord1].present) {
                const auto uv_address = address(texcoord1, vertex);
                if (attributes[texcoord1].format == half2_format) {
                    out.uv1 = {half_to_float(g.at<std::uint16_t>(uv_address)),
                        half_to_float(g.at<std::uint16_t>(uv_address + 2))};
                } else {
                    out.uv1 = g.at<std::array<float, 2>>(uv_address);
                }
            }
            for (const float coordinate : out.position) require(std::isfinite(coordinate), "a vertex is not finite");
            for (const float coordinate : out.uv0) require(std::isfinite(coordinate), "a UV0 coordinate is not finite");
            for (const float coordinate : out.uv1) require(std::isfinite(coordinate), "a UV1 coordinate is not finite");
            unsigned total{};
            for (std::size_t influence = 0; influence < influences; ++influence) {
                const auto high = influence >= 4;
                const auto weight = g.at<std::uint8_t>(address(high ? weights_high : weights_low, vertex) + influence % 4);
                if (!weight) continue; // an unused slot's bone need not be set
                const auto slot = g.at<std::uint16_t>(address(high ? bones_high : bones_low, vertex) + influence % 4 * 2);
                require(slot < bone_count, "a vertex names a bone outside its palette");
                out.bones[influence] = r.at<std::uint16_t>(bones + slot * 2u);
                out.weights[influence] = weight;
                total += weight;
            }
            require(total == 255, "a vertex's weights do not add up");
            result.vertices.push_back(out);
        }
        const std::size_t start = std::size_t{r.at<std::uint32_t>(s + first_index)} * 2;
        require(start + std::size_t{triangles} * 6 <= layout.index_bytes, "a section's indices run past the index buffer");
        for (std::size_t i = 0; i < std::size_t{triangles} * 3; ++i) {
            const auto index = g.at<std::uint16_t>(layout.vertex_bytes + start + i * 2);
            require(index < count, "a triangle names a vertex outside its section");
            result.triangles.push_back(first + index);
        }
    }
    return result;
}
}
