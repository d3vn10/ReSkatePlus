#include "mod_dump.h"

#include "game_archives.h"
#include "game_bundles.h"
#include "mod_list.h"
#include "Engine/Core/Platform/path_text.h"
#include "Engine/Resource/cas_codec.h"
#include "Engine/Resource/ebx_document.h"
#include "Engine/Resource/mesh_set.h"
#include "Engine/Resource/texture.h"
#include "Engine/Resource/toc.h"

#include <Windows.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <fstream>
#include <limits>
#include <memory>
#include <optional>
#include <set>
#include <span>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace dingosdk::mods {
namespace fs = std::filesystem;
namespace fb = frostbite;

namespace {
void write_file(const fs::path& path, std::span<const std::byte> bytes);

fs::path executable_directory() {
	std::array<wchar_t, 32768> path{};
	const auto length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
	if (!length || length >= path.size()) throw std::runtime_error("Cannot resolve the executable directory");
	return fs::path(std::wstring(path.data(), length)).parent_path();
}

void append_u32_be(std::vector<std::byte>& bytes, std::uint32_t value) {
	for (int shift = 24; shift >= 0; shift -= 8) bytes.push_back(static_cast<std::byte>(value >> shift));
}

void append_u32_le(std::vector<std::byte>& bytes, std::uint32_t value) {
	for (int shift = 0; shift <= 24; shift += 8) bytes.push_back(static_cast<std::byte>(value >> shift));
}

std::string json_string(std::string_view value) {
	std::string result = "\"";
	for (const auto ch : value) {
		if (ch == '\\' || ch == '"') { result.push_back('\\'); result.push_back(ch); }
		else if (static_cast<unsigned char>(ch) < 0x20) result += " ";
		else result.push_back(ch);
	}
	result.push_back('"');
	return result;
}

void write_glb(const fs::path& path, const fb::SkinnedMesh& mesh, std::string_view name) {
	if (mesh.vertices.empty() || mesh.triangles.empty() || mesh.triangles.size() % 3)
		throw std::runtime_error("MeshSet has no complete triangle geometry");
	if (!mesh.has_uv0) throw std::runtime_error("MeshSet has no supported UV0 vertex channel");
	if (!mesh.has_uv1) throw std::runtime_error("MeshSet has no supported UV1 vertex channel");
	std::vector<std::byte> binary;
	std::array<float, 3> minimum{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
	std::array<float, 3> maximum{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};
	for (const auto& vertex : mesh.vertices) {
		const std::array<float, 3> converted{vertex.position[0], vertex.position[2], -vertex.position[1]};
		for (std::size_t axis = 0; axis < converted.size(); ++axis) {
			minimum[axis] = std::min(minimum[axis], converted[axis]);
			maximum[axis] = std::max(maximum[axis], converted[axis]);
			append_u32_le(binary, std::bit_cast<std::uint32_t>(converted[axis]));
		}
	}
	const auto uv_offset = binary.size();
	for (const auto& vertex : mesh.vertices)
		for (const auto coordinate : vertex.uv0) append_u32_le(binary, std::bit_cast<std::uint32_t>(coordinate));
	const auto uv1_offset = binary.size();
	for (const auto& vertex : mesh.vertices)
		for (const auto coordinate : vertex.uv1) append_u32_le(binary, std::bit_cast<std::uint32_t>(coordinate));
	const auto color_offset = binary.size();
	if (mesh.has_source_id_color)
		for (const auto& vertex : mesh.vertices)
			for (const auto component : vertex.source_id_color) binary.push_back(static_cast<std::byte>(component));
	const auto index_offset = binary.size();
	for (const auto index : mesh.triangles) {
		if (index >= mesh.vertices.size()) throw std::runtime_error("MeshSet triangle index is invalid");
		append_u32_le(binary, index);
	}
	while (binary.size() % 4) binary.push_back(std::byte{});
	auto json = std::string("{\"accessors\":[{\"bufferView\":0,\"componentType\":5126,\"count\":") +
		std::to_string(mesh.vertices.size()) + ",\"max\":[" + std::to_string(maximum[0]) + "," +
		std::to_string(maximum[1]) + "," + std::to_string(maximum[2]) + "],\"min\":[" +
		std::to_string(minimum[0]) + "," + std::to_string(minimum[1]) + "," + std::to_string(minimum[2]) +
		"],\"type\":\"VEC3\"},{\"bufferView\":1,\"componentType\":5126,\"count\":" +
		std::to_string(mesh.vertices.size()) + ",\"type\":\"VEC2\"},{\"bufferView\":2,\"componentType\":5126,\"count\":" +
		std::to_string(mesh.vertices.size()) + ",\"type\":\"VEC2\"}";
	if (mesh.has_source_id_color)
		json += ",{\"bufferView\":3,\"componentType\":5121,\"count\":" +
			std::to_string(mesh.vertices.size()) + ",\"normalized\":true,\"type\":\"VEC4\"}";
	const auto index_accessor = mesh.has_source_id_color ? 4 : 3;
	const auto index_view = mesh.has_source_id_color ? 4 : 3;
	json += ",{\"bufferView\":" + std::to_string(index_view) + ",\"componentType\":5125,\"count\":" +
		std::to_string(mesh.triangles.size()) + ",\"max\":[" +
		std::to_string(*std::max_element(mesh.triangles.begin(), mesh.triangles.end())) + "],\"min\":[" +
		std::to_string(*std::min_element(mesh.triangles.begin(), mesh.triangles.end())) + "],\"type\":\"SCALAR\"}]," +
		"\"asset\":{\"generator\":\"ReSkatePlus mod dump\",\"version\":\"2.0\"},\"bufferViews\":[{" +
		"\"buffer\":0,\"byteLength\":" + std::to_string(uv_offset) + ",\"byteOffset\":0,\"target\":34962},{" +
		"\"buffer\":0,\"byteLength\":" + std::to_string(uv1_offset - uv_offset) + ",\"byteOffset\":" +
		std::to_string(uv_offset) + ",\"target\":34962},{" +
		"\"buffer\":0,\"byteLength\":" + std::to_string(color_offset - uv1_offset) + ",\"byteOffset\":" +
		std::to_string(uv1_offset) + ",\"target\":34962}";
	if (mesh.has_source_id_color)
		json += ",{\"buffer\":0,\"byteLength\":" + std::to_string(index_offset - color_offset) +
			",\"byteOffset\":" + std::to_string(color_offset) + ",\"target\":34962}";
	json += ",{\"buffer\":0,\"byteLength\":" + std::to_string(mesh.triangles.size() * 4) + ",\"byteOffset\":" +
		std::to_string(index_offset) + ",\"target\":34963}],\"buffers\":[{\"byteLength\":" +
		std::to_string(binary.size()) + "}],\"meshes\":[{\"name\":" + json_string(name) +
		",\"primitives\":[{\"attributes\":{\"POSITION\":0,\"TEXCOORD_0\":1,\"TEXCOORD_1\":2";
	if (mesh.has_source_id_color) json += ",\"COLOR_0\":3";
	json += "},\"indices\":" + std::to_string(index_accessor) + ",\"mode\":4}]}]," +
		"\"nodes\":[{\"mesh\":0,\"name\":" + json_string(name) +
		"}],\"scene\":0,\"scenes\":[{\"nodes\":[0]}]}";
	std::vector<std::byte> json_bytes;
	for (const auto ch : json) json_bytes.push_back(static_cast<std::byte>(ch));
	while (json_bytes.size() % 4) json_bytes.push_back(std::byte{' '});
	std::vector<std::byte> glb{std::byte{'g'}, std::byte{'l'}, std::byte{'T'}, std::byte{'F'}};
	append_u32_le(glb, 2);
	append_u32_le(glb, static_cast<std::uint32_t>(12 + 8 + json_bytes.size() + 8 + binary.size()));
	append_u32_le(glb, static_cast<std::uint32_t>(json_bytes.size()));
	append_u32_le(glb, 0x4E4F534A);
	glb.insert(glb.end(), json_bytes.begin(), json_bytes.end());
	append_u32_le(glb, static_cast<std::uint32_t>(binary.size()));
	append_u32_le(glb, 0x004E4942);
	glb.insert(glb.end(), binary.begin(), binary.end());
	write_file(path, glb);
}

std::uint32_t png_crc(std::span<const std::byte> bytes) {
	std::uint32_t crc = 0xFFFFFFFFU;
	for (const auto byte : bytes) {
		crc ^= std::to_integer<std::uint8_t>(byte);
		for (int bit = 0; bit < 8; ++bit) crc = (crc >> 1) ^ (0xEDB88320U & (0U - (crc & 1U)));
	}
	return ~crc;
}

void png_chunk(std::vector<std::byte>& png, std::string_view type, std::span<const std::byte> data) {
	if (data.size() > std::numeric_limits<std::uint32_t>::max()) throw std::runtime_error("PNG chunk is too large");
	append_u32_be(png, static_cast<std::uint32_t>(data.size()));
	const auto begin = png.size();
	for (const auto ch : type) png.push_back(static_cast<std::byte>(ch));
	png.insert(png.end(), data.begin(), data.end());
	append_u32_be(png, png_crc(std::span(png).subspan(begin)));
}

void write_png(const fs::path& path, const fb::Image& image) {
	if (!image.width || !image.height || image.rgba.size() != std::size_t{image.width} * image.height * 4)
		throw std::runtime_error("Texture image dimensions are invalid");
	std::vector<std::byte> raw;
	raw.reserve((std::size_t{image.width} * 4 + 1) * image.height);
	for (std::uint32_t y = 0; y < image.height; ++y) {
		raw.push_back(std::byte{});
		const auto* row = image.rgba.data() + std::size_t{y} * image.width * 4;
		for (std::size_t x = 0; x < std::size_t{image.width} * 4; ++x) raw.push_back(static_cast<std::byte>(row[x]));
	}
	std::vector<std::byte> compressed{std::byte{0x78}, std::byte{0x01}};
	std::size_t offset{};
	while (offset < raw.size()) {
		const auto count = static_cast<std::uint16_t>(std::min<std::size_t>(raw.size() - offset, 65535));
		compressed.push_back(static_cast<std::byte>(offset + count == raw.size() ? 1 : 0));
		compressed.push_back(static_cast<std::byte>(count));
		compressed.push_back(static_cast<std::byte>(count >> 8));
		const auto inverse = static_cast<std::uint16_t>(~count);
		compressed.push_back(static_cast<std::byte>(inverse));
		compressed.push_back(static_cast<std::byte>(inverse >> 8));
		compressed.insert(compressed.end(), raw.begin() + static_cast<std::ptrdiff_t>(offset),
			raw.begin() + static_cast<std::ptrdiff_t>(offset + count));
		offset += count;
	}
	std::uint32_t a = 1, b{};
	for (const auto value : raw) {
		a = (a + std::to_integer<std::uint8_t>(value)) % 65521U;
		b = (b + a) % 65521U;
	}
	append_u32_be(compressed, (b << 16) | a);
	std::vector<std::byte> png{std::byte{0x89}, std::byte{'P'}, std::byte{'N'}, std::byte{'G'},
		std::byte{0x0D}, std::byte{0x0A}, std::byte{0x1A}, std::byte{0x0A}};
	std::vector<std::byte> header;
	append_u32_be(header, image.width);
	append_u32_be(header, image.height);
	header.insert(header.end(), {std::byte{8}, std::byte{6}, std::byte{}, std::byte{}, std::byte{}});
	png_chunk(png, "IHDR", header);
	png_chunk(png, "IDAT", compressed);
	png_chunk(png, "IEND", {});
	write_file(path, png);
}

void collect_resource_references(const fb::ebx::Value& value, std::set<std::uint64_t>& result) {
	if (const auto* reference = std::get_if<fb::ebx::ResourceReference>(&value.data)) {
		if (reference->id) result.insert(reference->id);
	} else if (const auto* object = std::get_if<std::shared_ptr<fb::ebx::Object>>(&value.data); object && *object) {
		for (const auto& field : (*object)->fields) collect_resource_references(field.value, result);
	} else if (const auto* array = std::get_if<fb::ebx::Value::Array>(&value.data)) {
		for (const auto& element : *array) collect_resource_references(element, result);
	}
}

std::string lower(std::string value) {
	for (auto& ch : value) if (ch >= 'A' && ch <= 'Z') ch = static_cast<char>(ch + ('a' - 'A'));
	return value;
}

void report_progress(const ModDumpProgress& progress, std::string_view status) {
	if (progress) progress(status);
}

bool is_inside_or_same(const fs::path& candidate, const fs::path& root) {
	std::error_code error;
	const auto resolved_candidate = fs::weakly_canonical(candidate, error);
	if (error) throw std::runtime_error("Cannot resolve dump folder: " + error.message());
	const auto resolved_root = fs::weakly_canonical(root, error);
	if (error) throw std::runtime_error("Cannot resolve mod folder: " + error.message());
	if (resolved_candidate == resolved_root) return true;
	const auto relative = resolved_candidate.lexically_relative(resolved_root);
	return !relative.empty() && *relative.begin() != "..";
}

fs::path safe_relative(std::string_view value) {
	if (value.empty() || value.find('\\') != std::string_view::npos || value.find('\0') != std::string_view::npos)
		throw std::runtime_error("Mod contains an unsafe asset path");
	const fs::path path(value);
	if (path.is_absolute() || path.has_root_name() || path.has_root_directory())
		throw std::runtime_error("Mod contains an absolute asset path");
	for (const auto& part : path) {
		const auto text = part.string();
		if (text.empty() || text == "." || text == ".." || text.find(':') != std::string::npos)
			throw std::runtime_error("Mod contains an unsafe asset path");
	}
	return path;
}

std::vector<std::byte> read_file(const fs::path& path) {
	std::ifstream input(path, std::ios::binary | std::ios::ate);
	if (!input) throw std::runtime_error("Cannot open " + path_utf8(path));
	const auto length = input.tellg();
	if (length < 0) throw std::runtime_error("Cannot read " + path_utf8(path));
	std::vector<std::byte> bytes(static_cast<std::size_t>(length));
	input.seekg(0);
	if (!bytes.empty() && !input.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size())))
		throw std::runtime_error("Cannot read " + path_utf8(path));
	return bytes;
}

void write_file(const fs::path& path, std::span<const std::byte> bytes) {
	fs::create_directories(path.parent_path());
	std::ofstream output(path, std::ios::binary | std::ios::trunc);
	if (!output || (!bytes.empty() && !output.write(reinterpret_cast<const char*>(bytes.data()),
			static_cast<std::streamsize>(bytes.size()))) || !output.flush())
		throw std::runtime_error("Cannot write " + path_utf8(path));
}

fs::path toc_relative(std::string_view name) {
	auto relative = safe_relative(name);
	if (lower(relative.extension().string()) != ".toc") relative += ".toc";
	return relative;
}

using GameEbxIndex = std::unordered_map<std::string, std::vector<fb::Sha1>>;

GameEbxIndex index_game_ebx(const fs::path& game_root, const vfs::GameData& game_data) {
	GameEbxIndex index;
	const auto layout = vfs::read_layout(game_root / L"Data" / L"layout.toc");
	const auto* super_bundles = layout.root.field("superBundles");
	if (!super_bundles || super_bundles->type != 1) return index;

	std::set<std::string, std::less<>> seen_tocs;
	std::error_code error;
	for (const auto& row : super_bundles->children) {
		const auto* name = row.field("name");
		if (!name || name->type != 7 || name->text.empty()) continue;
		const auto relative = toc_relative(name->text);
		if (!seen_tocs.emplace(lower(relative.generic_string())).second) continue;
		if (!fs::is_regular_file(game_root / L"Data" / relative, error)) {
			if (error && error != std::errc::no_such_file_or_directory)
				throw std::runtime_error("Cannot inspect game TOC " + path_utf8(game_root / L"Data" / relative) + ": " + error.message());
			error.clear();
			continue;
		}
		const auto toc = game_data.read_toc(relative.generic_string());
		for (const auto& toc_bundle : toc.bundles) {
			const auto bundle = game_data.read_bundle(toc, toc_bundle.name);
			if (!bundle) continue;
			for (const auto& asset : bundle->manifest.ebx)
				index[lower(asset.name)].push_back(asset.sha1);
		}
	}
	return index;
}

void reject_symlink(const fs::path& path) {
	std::error_code error;
	const auto status = fs::symlink_status(path, error);
	if (!error && fs::is_symlink(status)) throw std::runtime_error("Dump path contains a symbolic link: " + path_utf8(path));
	if (error && error != std::errc::no_such_file_or_directory)
		throw std::runtime_error("Cannot inspect dump path " + path_utf8(path) + ": " + error.message());
}

bool export_ebx(const fs::path& output, const std::string& asset_name,
				const vfs::GameBundle& bundle, vfs::GameArchives& archives, std::size_t index,
				const fs::path& mod_root, const fs::path& game_root, const ModDumpProgress& progress,
				std::string& root_type, fs::path& exported_path, std::set<std::uint64_t>& references) {
	const auto* payload = bundle.payload(fb::AssetKind::ebx, index);
	if (!payload) throw std::runtime_error("Bundle manifest has no payload for " + asset_name);
	if (!payload->location.patch) return false;
	const auto archive_size = archives.archive_size(mod_root, payload->location);
	if (!archive_size || payload->offset > *archive_size || payload->size > *archive_size - payload->offset)
		return false;
	std::string status = "Extracting ";
	status += asset_name;
	status += " from ";
	status += path_utf8(mod_root / L"Win32" / fs::path(archives.describe(payload->location)));
	report_progress(progress, status);
	const auto raw = archives.read(mod_root, payload->location, payload->offset, payload->size);
	const auto decoded = fb::decode_cas(raw, {.gameRoot = game_root});
	try {
		const auto document = fb::ebx::read_document(decoded);
		root_type = document.rootType;
		for (const auto& instance : document.instances)
			if (instance.object)
				for (const auto& field : instance.object->fields) collect_resource_references(field.value, references);
	}
	catch (const std::exception&) { root_type = "Unknown"; }
	auto relative = safe_relative(asset_name);
	if (!lower(relative.filename().string()).ends_with(".ebx")) relative += ".ebx";
	exported_path = output / relative;
	write_file(exported_path, decoded);
	return true;
}

std::vector<std::byte> read_layer_payload(vfs::GameArchives& mod_archives, const fs::path& mod_root,
										 vfs::GameArchives* game_archives, const fs::path& game_root,
										 const fb::BundleFileInfo& payload) {
	if (payload.location.patch) return fb::decode_cas(mod_archives.read(mod_root, payload.location,
		payload.offset, payload.size), {.gameRoot = game_root});
	if (!game_archives) throw std::runtime_error("Referenced asset belongs to the game archives");
	return fb::decode_cas(game_archives->read(game_archives->root(), payload.location, payload.offset, payload.size),
		{.gameRoot = game_root});
}

	std::size_t export_referenced_textures(const fs::path& ebx_path,
										  const std::set<std::uint64_t>& references, const vfs::GameBundle& bundle,
										  const fb::TocDocument& toc, vfs::GameArchives& archives,
										  const fs::path& mod_root, const fs::path& game_root,
										  vfs::GameArchives* game_archives) {
	std::size_t exported{};
	for (std::size_t index = 0; index < bundle.manifest.resources.size(); ++index) {
		const auto& resource = bundle.manifest.resources[index];
		if (!resource.resourceId || !references.contains(resource.resourceId) ||
			resource.resourceType != fb::texture_resource_type) continue;
		const auto* resource_payload = bundle.payload(fb::AssetKind::resource, index);
		if (!resource_payload) continue;
		try {
			const auto header_bytes = read_layer_payload(archives, mod_root, game_archives, game_root, *resource_payload);
			const auto header = fb::read_texture_header(header_bytes, resource.resourceMeta);
			std::vector<std::byte> pixels;
			const auto toc_chunk = std::find_if(toc.chunks.begin(), toc.chunks.end(), [&](const fb::TocChunk& chunk) {
				return chunk.guid == header.chunk && !chunk.removed;
			});
			if (toc_chunk != toc.chunks.end()) {
				pixels = read_layer_payload(archives, mod_root, game_archives, game_root,
					{toc_chunk->location, toc_chunk->offset, toc_chunk->size});
			} else {
				std::size_t chunk_index{};
				const auto* chunk = bundle.find_chunk(header.chunk, &chunk_index);
				const auto* chunk_payload = chunk ? bundle.payload(fb::AssetKind::chunk, chunk_index) : nullptr;
				if (!chunk_payload) continue;
				pixels = read_layer_payload(archives, mod_root, game_archives, game_root, *chunk_payload);
			}
			const auto image = fb::decode_texture(header, pixels, std::min(header.width, header.height));
			auto companion = ebx_path.parent_path() /
				(ebx_path.stem().string() + "__" + safe_relative(resource.name).stem().string() + ".png");
			write_png(companion, image);
			++exported;
		} catch (const std::exception&) {
			continue;
		}
	}
	return exported;
}

std::size_t export_referenced_meshes(const fs::path& ebx_path,
										const std::set<std::uint64_t>& references, const vfs::GameBundle& bundle,
										const fb::TocDocument& toc, vfs::GameArchives& archives,
										const fs::path& mod_root, const fs::path& game_root,
										vfs::GameArchives* game_archives, const ModDumpProgress& progress) {
	static constexpr std::uint32_t mesh_set_type = 0x49b156d4;
	std::size_t exported{};
	for (std::size_t resource_index = 0; resource_index < bundle.manifest.resources.size(); ++resource_index) {
		const auto& resource = bundle.manifest.resources[resource_index];
		if (!resource.resourceId || !references.contains(resource.resourceId) || resource.resourceType != mesh_set_type) continue;
		try {
			const auto* resource_payload = bundle.payload(fb::AssetKind::resource, resource_index);
			if (!resource_payload) continue;
			const auto mesh_resource = read_layer_payload(archives, mod_root, game_archives, game_root, *resource_payload);
			const auto lod_count = std::min<std::size_t>(fb::mesh_lod_count(mesh_resource), 3);
			for (std::size_t lod = 0; lod < lod_count; ++lod) {
				try {
					const auto mesh_lod = fb::read_mesh_lod(mesh_resource, lod);
					std::vector<std::byte> geometry;
					std::size_t chunk_index{};
					if (bundle.find_chunk(mesh_lod.chunk, &chunk_index)) {
						if (const auto* payload = bundle.payload(fb::AssetKind::chunk, chunk_index))
							geometry = read_layer_payload(archives, mod_root, game_archives, game_root, *payload);
					}
					if (geometry.empty()) {
						const auto chunk = std::find_if(toc.chunks.begin(), toc.chunks.end(), [&](const fb::TocChunk& entry) {
							return entry.guid == mesh_lod.chunk && !entry.removed;
						});
						if (chunk == toc.chunks.end()) continue;
						geometry = read_layer_payload(archives, mod_root, game_archives, game_root,
							{chunk->location, chunk->offset, chunk->size});
					}
					const auto mesh = fb::read_skinned_mesh(mesh_resource, lod, geometry);
					const auto output = ebx_path.parent_path() /
						(ebx_path.stem().string() + "__" + safe_relative(resource.name).stem().string() +
						"_lod" + std::to_string(lod) + ".glb");
					write_glb(output, mesh, resource.name + " LOD " + std::to_string(lod));
					++exported;
				} catch (const std::exception& error) {
					report_progress(progress, "Skipping mesh " + resource.name + " LOD " + std::to_string(lod) + ": " + error.what());
					continue;
				}
			}
		} catch (const std::exception& error) {
			report_progress(progress, "Skipping mesh " + resource.name + ": " + error.what());
			continue;
		}
	}
	return exported;
}

ModDumpReport dump_mod_at(const fs::path& mod_root, const fs::path& game_root, const fs::path& output,
						  const ModDumpProgress& progress, bool replace_output, bool& output_created) {
	if (!fs::is_directory(mod_root)) throw std::runtime_error("Mod folder does not exist: " + path_utf8(mod_root));
	report_progress(progress, "Dumping selected mod from " + path_utf8(mod_root));
	report_progress(progress, "Reading layout.toc");
	auto layout = vfs::read_layout(mod_root / L"layout.toc");
	vfs::GameArchives archives(mod_root, layout.root);
	const auto game_layout_path = game_root / L"Data" / L"layout.toc";
	std::unique_ptr<vfs::GameArchives> game_archives;
	if (fs::is_regular_file(game_layout_path)) {
		auto game_layout = vfs::read_layout(game_layout_path);
		game_archives = std::make_unique<vfs::GameArchives>(game_root / L"Data", game_layout.root);
	}
	std::unique_ptr<vfs::GameData> game_data;
	if (fs::is_regular_file(game_layout_path)) game_data = std::make_unique<vfs::GameData>(game_root);
	const auto game_ebx = game_data ? index_game_ebx(game_root, *game_data) : GameEbxIndex{};
	const auto* super_bundles = layout.root.field("superBundles");
	if (!super_bundles || super_bundles->type != 1)
		throw std::runtime_error("layout.toc has no superBundles list");

	std::error_code error;
	if (replace_output) {
		reject_symlink(output.parent_path().parent_path().parent_path());
		reject_symlink(output.parent_path().parent_path());
		reject_symlink(output.parent_path());
		fs::create_directories(output.parent_path(), error);
		if (error) throw std::runtime_error("Cannot create dump folder: " + error.message());
		reject_symlink(output);
		fs::remove_all(output, error);
		if (error) throw std::runtime_error("Cannot replace dump folder: " + error.message());
		fs::create_directory(output, error);
	} else {
		if (is_inside_or_same(output, mod_root)) throw std::runtime_error("Dump folder cannot be inside the mod folder");
		fs::create_directories(output.parent_path(), error);
		if (error) throw std::runtime_error("Cannot create dump folder: " + error.message());
		if (!fs::create_directory(output, error) && !error)
			throw std::runtime_error("Dump folder already exists: " + path_utf8(output));
	}
	if (error) throw std::runtime_error("Cannot create dump folder: " + error.message());
	output_created = true;

	ModDumpReport report;
	report.directory = output;
	const auto merge_targets_path = output / L"merge_targets.txt";
	std::ofstream merge_targets(merge_targets_path, std::ios::binary | std::ios::trunc);
	if (!merge_targets) throw std::runtime_error("Cannot write " + path_utf8(merge_targets_path));
	merge_targets << "EBX merge destinations for this mod. Other enabled mods and load order can affect the final result.\n"
		<< "Added/replaced is determined by matching the asset name in the same game bundle.\n\n";

	std::set<std::string, std::less<>> seen_tocs;
	for (const auto& row : super_bundles->children) {
		const auto* name = row.field("name");
		if (!name || name->type != 7 || name->text.empty()) continue;
		const auto relative = toc_relative(name->text);
		const auto key = lower(relative.generic_string());
		if (!seen_tocs.emplace(key).second) continue;
		const auto toc_path = mod_root / relative;
		if (!fs::is_regular_file(toc_path, error)) {
			if (error && error != std::errc::no_such_file_or_directory)
				throw std::runtime_error("Cannot inspect " + path_utf8(toc_path) + ": " + error.message());
			error.clear();
			continue;
		}
		report_progress(progress, "Reading " + relative.generic_string());
		const auto toc = fb::read_toc(read_file(toc_path));
		std::optional<fb::TocDocument> base_toc;
		if (game_data && fs::is_regular_file(game_root / L"Data" / relative))
			base_toc = game_data->read_toc(relative.generic_string());
		for (const auto& toc_bundle : toc.bundles) {
			std::string status = "Reading bundle ";
			status += toc_bundle.name;
			status += " from ";
			status += path_utf8(toc_path);
			report_progress(progress, status);
			(void)safe_relative(toc_bundle.name);
			const auto region = fb::read_bundle_region(toc_bundle.region);
			vfs::GameBundle bundle;
			if (!region.inlineManifest.empty()) {
				bundle.manifest = fb::read_binary_bundle(region.inlineManifest);
				bundle.payloads = region.files;
			} else {
				if (region.files.empty()) throw std::runtime_error("Bundle " + toc_bundle.name + " has no manifest");
				const auto& manifest_record = region.files.front();
				std::vector<std::byte> manifest_bytes;
				try {
					manifest_bytes = archives.read(mod_root, manifest_record.location, manifest_record.offset, manifest_record.size);
				} catch (const std::runtime_error&) {
					if (!game_archives) throw;
					manifest_bytes = game_archives->read(game_archives->root(), manifest_record.location, manifest_record.offset, manifest_record.size);
				}
				try { bundle.manifest = fb::read_binary_bundle(manifest_bytes); }
				catch (const std::exception&) { bundle.manifest = fb::read_binary_bundle(fb::decode_cas(manifest_bytes, {.gameRoot = game_root})); }
				bundle.payloads.assign(region.files.begin() + 1, region.files.end());
			}
			std::optional<vfs::GameBundle> base_bundle;
			if (game_data && base_toc) base_bundle = game_data->read_bundle(*base_toc, toc_bundle.name);
			for (std::size_t i = 0; i < bundle.manifest.ebx.size(); ++i) {
				const auto& asset = bundle.manifest.ebx[i];
				const auto* original = base_bundle ? base_bundle->find(fb::AssetKind::ebx, asset.name) : nullptr;
				if (base_bundle) {
					if (original && original->sha1 == asset.sha1) continue;
				}
				if (const auto anywhere = game_ebx.find(lower(asset.name)); anywhere != game_ebx.end() &&
					std::find(anywhere->second.begin(), anywhere->second.end(), asset.sha1) != anywhere->second.end()) continue;
				std::string root_type;
				fs::path ebx_path;
				std::set<std::uint64_t> references;
				if (export_ebx(output, asset.name, bundle, archives, i, mod_root, game_root, progress,
					root_type, ebx_path, references)) {
					++report.ebx;
					report.resources += export_referenced_textures(ebx_path, references, bundle, toc,
						archives, mod_root, game_root, game_archives.get());
					report.meshes += export_referenced_meshes(ebx_path, references, bundle, toc,
						archives, mod_root, game_root, game_archives.get(), progress);
					merge_targets << path_utf8(mod_root.filename()) << " [" << root_type << "] "
						<< (original ? "REPLACES " : "ADDS ") << asset.name
						<< " -> " << relative.generic_string() << " :: " << toc_bundle.name;
					if (original) merge_targets << " (game asset: " << original->name << ')';
					merge_targets << '\n';
				}
			}
		}
	}
	merge_targets.flush();
	if (!merge_targets) throw std::runtime_error("Cannot write " + path_utf8(merge_targets_path));
	report_progress(progress, "Dump complete");
	return report;
}

ModDumpReport dump_mod_staged(const fs::path& mod_root, const fs::path& game_root, const fs::path& output,
		const ModDumpProgress& progress, bool replace_output) {
	static std::atomic_uint64_t sequence{};
	std::error_code error;
	fs::create_directories(output.parent_path(), error);
	if (error) throw std::runtime_error("Cannot create dump folder: " + error.message());
	reject_symlink(output);
	if (!replace_output && fs::exists(output)) throw std::runtime_error("Dump folder already exists: " + path_utf8(output));
	fs::path staging;
	for (unsigned attempt = 0; attempt < 32; ++attempt) {
		staging = output.parent_path() / (".reskate-dump-" + std::to_string(GetCurrentProcessId()) + "-" +
			std::to_string(sequence.fetch_add(1, std::memory_order_relaxed)));
		if (fs::create_directory(staging, error)) break;
		if (error && error != std::errc::file_exists)
			throw std::runtime_error("Cannot create temporary dump folder: " + error.message());
		staging.clear();
		error.clear();
	}
	if (staging.empty()) throw std::runtime_error("Cannot allocate a temporary dump folder");
	fs::remove(staging, error);
	if (error) throw std::runtime_error("Cannot prepare temporary dump folder: " + error.message());
	bool staging_created{};
	fs::path backup;
	try {
		auto report = dump_mod_at(mod_root, game_root, staging, progress, false, staging_created);
		if (replace_output && fs::exists(output)) {
			backup = output.parent_path() / (".reskate-dump-backup-" + std::to_string(GetCurrentProcessId()) + "-" +
				std::to_string(sequence.fetch_add(1, std::memory_order_relaxed)));
			fs::rename(output, backup);
		}
		try {
			fs::rename(staging, output);
			staging_created = false;
		} catch (...) {
			if (!backup.empty()) fs::rename(backup, output);
			backup.clear();
			throw;
		}
		if (!backup.empty()) fs::remove_all(backup, error);
		report.directory = output;
		return report;
	} catch (...) {
		if (staging_created) fs::remove_all(staging, error);
		if (!backup.empty() && !fs::exists(output)) fs::rename(backup, output, error);
		throw;
	}
}
}

ModDumpReport dump_mod(std::string_view name) {
	if (!valid_mod_name(name)) throw std::invalid_argument("Invalid mod folder name");
	const auto game_root = executable_directory();
	const auto mods_root = game_root / L"Mods";
	const auto mod_root = mods_root / fs::path(std::string(name));
	const auto output = mods_root / L".reskate" / L"dumps" / fs::path(std::string(name));
	return dump_mod_staged(mod_root, game_root, output, {}, true);
}

ModDumpReport dump_mod(const fs::path& mod_directory, const fs::path& output_directory,
					   const ModDumpProgress& progress, const fs::path& game_root) {
	if (output_directory.empty()) throw std::invalid_argument("Dump folder is empty");
	const auto selected_game_root = game_root.empty() ? executable_directory() : game_root;
	return dump_mod_staged(mod_directory, selected_game_root, output_directory, progress, false);
}

}
