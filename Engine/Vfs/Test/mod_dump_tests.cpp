#include "Engine/Vfs/mod_dump.h"
#include "Engine/Vfs/native_db.h"
#include "Engine/Core/Platform/path_text.h"
#include "Engine/Resource/binary_bundle.h"
#include "Engine/Resource/cas_codec.h"
#include "Engine/Resource/ebx_writer.h"
#include "Engine/Resource/mesh_set.h"
#include "Engine/Resource/toc.h"
#include "Engine/Resource/texture.h"

#include <Windows.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

using namespace dingosdk;
namespace fs = std::filesystem;
namespace db = dingosdk::native_db;
namespace fb = dingosdk::frostbite;

namespace {
void check(bool value, const char* message) {
	if (!value) throw std::runtime_error(message);
}

db::Node leaf(unsigned type, const char* name, std::vector<unsigned char> payload) {
	db::Node node;
	node.type = type;
	node.named = true;
	node.name = name;
	node.owned = std::move(payload);
	return node;
}

db::Node container(unsigned type, const char* name, std::vector<db::Node> children) {
	db::Node node;
	node.type = type;
	node.named = *name != '\0';
	node.name = name;
	node.terminated = true;
	node.children = std::move(children);
	return node;
}

void write_bytes(const fs::path& path, std::span<const std::byte> bytes) {
	fs::create_directories(path.parent_path());
	std::ofstream output(path, std::ios::binary | std::ios::trunc);
	if (!output || (!bytes.empty() && !output.write(reinterpret_cast<const char*>(bytes.data()),
			static_cast<std::streamsize>(bytes.size()))))
		throw std::runtime_error("could not write fixture file");
}

void write_bytes(const fs::path& path, const std::vector<unsigned char>& bytes) {
	write_bytes(path, std::as_bytes(std::span(bytes)));
}

std::vector<std::byte> read_bytes(const fs::path& path) {
	std::ifstream input(path, std::ios::binary | std::ios::ate);
	if (!input) throw std::runtime_error("could not read exported file: " + path.string());
	const auto size = static_cast<std::size_t>(input.tellg());
	std::vector<std::byte> bytes(size);
	input.seekg(0);
	if (size && !input.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(size)))
		throw std::runtime_error("could not read exported file");
	return bytes;
}

void check_file_bytes(const fs::path& root, const fs::path& relative, const std::vector<std::byte>& expected) {
	const auto path = root / relative;
	check(fs::is_regular_file(path), ("expected output file is missing: " + path.string()).c_str());
	check(path.extension() == L".ebx", ("output file does not have .ebx extension: " + path.string()).c_str());
	check(read_bytes(path) == expected, ("output bytes differ from expected EBX payload: " + path.string()).c_str());
	std::cout << "Verified EBX: " << path.string() << " (" << expected.size() << " bytes)\n";
}

fb::Guid guid_from_text(std::string_view text) {
	std::array<std::byte, 16> canonical{};
	std::size_t digit{};
	const auto nibble = [](char ch) -> unsigned {
		if (ch >= '0' && ch <= '9') return static_cast<unsigned>(ch - '0');
		if (ch >= 'a' && ch <= 'f') return static_cast<unsigned>(ch - 'a' + 10);
		if (ch >= 'A' && ch <= 'F') return static_cast<unsigned>(ch - 'A' + 10);
		throw std::invalid_argument("invalid GUID in test fixture");
	};
	for (const auto ch : text) {
		if (ch == '-') continue;
		const auto value = nibble(ch);
		if (digit >= canonical.size() * 2) throw std::invalid_argument("long GUID in test fixture");
		if ((digit & 1U) == 0) canonical[digit / 2] = static_cast<std::byte>(value << 4);
		else canonical[digit / 2] |= static_cast<std::byte>(value);
		++digit;
	}
	if (digit != canonical.size() * 2) throw std::invalid_argument("short GUID in test fixture");
	fb::Guid guid;
	guid.bytes = {canonical[3], canonical[2], canonical[1], canonical[0], canonical[5], canonical[4],
		canonical[7], canonical[6], canonical[8], canonical[9], canonical[10], canonical[11],
		canonical[12], canonical[13], canonical[14], canonical[15]};
	return guid;
}

std::pair<std::uint32_t, std::uint32_t> append_encoded(std::vector<std::byte>& archive,
													   const std::vector<std::byte>& decoded) {
	const auto encoded = fb::encode_cas(decoded, {.compression = fb::CasCompression::raw});
	const auto offset = static_cast<std::uint32_t>(archive.size());
	const auto size = static_cast<std::uint32_t>(encoded.size());
	archive.insert(archive.end(), encoded.begin(), encoded.end());
	return {offset, size};
}

fb::ebx::Value resource_reference(std::uint64_t id) {
	fb::ebx::Value value;
	value.data = fb::ebx::ResourceReference{id};
	return value;
}

std::vector<std::byte> ebx_with_texture_reference(std::uint64_t resource_id) {
	fb::ebx::Document document;
	document.fileGuid = guid_from_text("a13018d8-cc09-4d5a-91b3-1895670ef0d7");
	document.rootType = "TextureOwner";
	fb::BinaryWriter reflection;
	reflection.u32(1);
	reflection.guid(guid_from_text("b23018d8-cc09-4d5a-91b3-1895670ef0d7"));
	reflection.u32(1);
	reflection.u32(1);
	reflection.u32(0);
	reflection.i32(0);
	reflection.u16(1);
	reflection.u16(0);
	reflection.u16(16);
	reflection.u16(8);
	reflection.u32(1);
	reflection.u32(0);
	reflection.u32(8);
	reflection.u16(static_cast<std::uint16_t>(((static_cast<std::uint16_t>(fb::ebx::FieldType::resourceRef) << 4) |
		static_cast<std::uint16_t>(fb::ebx::FieldCategory::primitive)) << 1));
	reflection.u16(0);
	reflection.u32(0);
	reflection.u32(0);
	for (const auto ch : std::string_view("TextureOwner\0Texture\0", 21)) reflection.u8(static_cast<std::uint8_t>(ch));
	document.reflectionData = reflection.take();
	document.types.resize(1);
	document.types[0].name = document.rootType;
	document.types[0].fieldIndex = 0;
	document.types[0].fieldCount = 1;
	document.types[0].size = 16;
	document.types[0].alignment = 8;
	document.types[0].guid = guid_from_text("b23018d8-cc09-4d5a-91b3-1895670ef0d7");
	document.fixupTypeGuids.push_back(document.types[0].guid);
	document.fixupTypeSignatures.push_back(1);
	fb::ebx::FieldDescriptor field;
	field.name = "Texture";
	field.encodedType = (static_cast<std::uint16_t>(fb::ebx::FieldType::resourceRef) << 4) |
		static_cast<std::uint16_t>(fb::ebx::FieldCategory::primitive);
	document.fields.push_back(field);
	auto object = std::make_shared<fb::ebx::Object>();
	object->descriptor = 0;
	object->fields.push_back({0, "Texture", resource_reference(resource_id)});
	fb::ebx::InstanceRecord root;
	root.fixupType = 0;
	root.descriptor = 0;
	root.exported = true;
	root.instanceGuid = guid_from_text("c23018d8-cc09-4d5a-91b3-1895670ef0d7");
	root.object = std::move(object);
	document.instances.push_back(std::move(root));
	return fb::ebx::write_document(document);
}

std::vector<std::byte> texture_header(const fb::Guid& chunk_guid) {
	fb::BinaryWriter writer;
	writer.seek(144);
	writer.seek(0);
	writer.u64(0);
	writer.u32(0);
	writer.i32(18);
	writer.u16(0);
	writer.u16(1);
	writer.u16(1);
	writer.u16(1);
	writer.u16(1);
	writer.u8(1);
	writer.u8(0);
	writer.u32(0);
	writer.guid(chunk_guid);
	for (int index = 0; index < 15; ++index) writer.u32(index == 0 ? 4 : 0);
	writer.u32(4);
	return writer.take();
}

void put_value(std::vector<std::byte>& data, std::size_t offset, auto value) {
	if (data.size() < offset + sizeof(value)) data.resize(offset + sizeof(value));
	std::memcpy(data.data() + offset, &value, sizeof(value));
}

std::pair<std::vector<std::byte>, std::vector<std::byte>> skinned_mesh_fixture(const fb::Guid& geometry_guid) {
	std::vector<std::byte> resource(1100);
	const std::array<std::uint32_t, 3> header{208, 188, 384};
	for (std::size_t index = 0; index < header.size(); ++index) put_value(resource, index * 4, header[index]);
	put_value(resource, std::size_t{124}, std::uint8_t{1});
	put_value(resource, std::size_t{180}, std::uint16_t{1});
	put_value(resource, std::size_t{48}, std::uint64_t{240});
	put_value(resource, std::size_t{256}, std::uint32_t{1});
	put_value(resource, std::size_t{264}, std::uint32_t{1});
	put_value(resource, std::size_t{268}, std::uint64_t{480});
	put_value(resource, std::size_t{340}, std::uint32_t{33});
	put_value(resource, std::size_t{344}, std::uint32_t{6});
	put_value(resource, std::size_t{348}, std::uint32_t{132});
	std::memcpy(resource.data() + 372, geometry_guid.bytes.data(), geometry_guid.bytes.size());
	put_value(resource, std::size_t{528}, std::uint64_t{1008});
	put_value(resource, std::size_t{536}, std::uint16_t{2});
	put_value(resource, std::size_t{543}, std::uint8_t{3});
	put_value(resource, std::size_t{544}, std::uint32_t{1});
	put_value(resource, std::size_t{556}, std::uint32_t{3});
	const std::size_t declaration = 624;
	const std::array<std::array<std::uint8_t, 4>, 6> elements{{
		{{1, 3, 0, 0}}, {{2, 23, 12, 0}}, {{3, 23, 20, 0}}, {{4, 13, 28, 0}}, {{5, 13, 32, 0}}, {{6, 2, 36, 0}}}};
	for (std::size_t index = 0; index < elements.size(); ++index)
		for (std::size_t component = 0; component < 4; ++component)
			put_value(resource, declaration + index * 4 + component, elements[index][component]);
	put_value(resource, declaration + 64, std::uint8_t{44});
	put_value(resource, declaration + 96, std::uint8_t{6});
	put_value(resource, declaration + 97, std::uint8_t{1});
	put_value(resource, std::size_t{1008}, std::uint16_t{0});
	put_value(resource, std::size_t{1010}, std::uint16_t{1});
	std::vector<std::byte> geometry(138);
	for (std::size_t vertex = 0; vertex < 3; ++vertex) {
		const auto at = vertex * 44;
		put_value(geometry, at, std::array<float, 3>{static_cast<float>(vertex), 0.0f, 0.0f});
		put_value(geometry, at + 12, std::array<std::uint16_t, 4>{0, 1, 0, 0});
		put_value(geometry, at + 20, std::array<std::uint16_t, 4>{0, 0, 0, 0});
		put_value(geometry, at + 28, std::array<std::uint8_t, 4>{200, 55, 0, 0});
		put_value(geometry, at + 32, std::array<std::uint8_t, 4>{0, 0, 0, 0});
		put_value(geometry, at + 36, std::array<float, 2>{static_cast<float>(vertex) / 2.0f, static_cast<float>(vertex) / 4.0f});
	}
	put_value(geometry, std::size_t{132}, std::array<std::uint16_t, 3>{0, 1, 2});
	return {std::move(resource), std::move(geometry)};
}

void the_mod_contents_dump() {
	std::array<wchar_t, 32768> executable{};
	const auto length = GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
	check(length && length < executable.size(), "test executable path is available");
	const auto game_root = fs::path(std::wstring(executable.data(), length)).parent_path();
	const auto mods_root = game_root / L"Mods";
	const auto name = "DumpFixture_" + std::to_string(GetCurrentProcessId());
	const auto mod = mods_root / name;
	const auto output = mods_root / L".reskate" / L"dumps" / name;
	const auto selected_parent = game_root / (L"DumpPicker_" + std::to_wstring(GetCurrentProcessId()));
	const auto selected_output = selected_parent / L"TestDumpMod";
	const auto fixture_game = selected_parent / L"game";
	std::error_code error;
	fs::remove_all(mod, error);
	fs::remove_all(output, error);
	fs::remove_all(selected_parent, error);

	const std::vector<std::byte> collection_ebx{std::byte{0x10}, std::byte{0x20}};
	auto card_ebx = std::vector<std::byte>{std::byte{0x30}, std::byte{0x40}};
	const std::vector<std::byte> cdn_ebx{std::byte{0x50}, std::byte{0x60}};
	const std::vector<std::byte> rime_ebx{std::byte{0x70}, std::byte{0x80}};
	const std::vector<std::byte> texture_ebx{std::byte{0x90}, std::byte{0xa0}};
	std::vector<std::byte> resolution_ebx{std::byte{0xb0}, std::byte{0xc0}};
	const std::vector<std::byte> rime_res{std::byte{0xd0}, std::byte{0xd1}};
	auto texture_res = std::vector<std::byte>{std::byte{0xd2}, std::byte{0xd3}};
	const std::vector<std::byte> resolution_res{std::byte{0xd4}, std::byte{0xd5}};
	const std::vector<std::byte> first_chunk{std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0xff}};
	const std::vector<std::byte> second_chunk{std::byte{0xe2}, std::byte{0xe3}};
	const std::vector<std::byte> game_collection{std::byte{0xf0}, std::byte{0xf1}};
	const std::string icon_root = "thumbnail/pcard/reskate/playercard_icon/own_test_pcard_icon";
	const std::string first_chunk_guid = "2a5018d8-cc09-4d5a-91b3-1895670ef0d7";
	const std::string second_chunk_guid = "ba72a419-498e-4db8-b653-cbb925a2c1b1";
	const std::string mesh_chunk_guid = "ca72a419-498e-4db8-b653-cbb925a2c1b1";
	card_ebx = ebx_with_texture_reference(0x12345678);
	resolution_ebx = ebx_with_texture_reference(0x23456789);
	const auto card_document = fb::ebx::read_document(card_ebx);
	check(card_document.rootType == "TextureOwner" && card_document.root() && card_document.root()->object &&
		std::get<fb::ebx::ResourceReference>(card_document.root()->object->fields.front().value.data).id == 0x12345678,
		"test EBX fixture exposes its referenced resource ID through reflection");
	texture_res = texture_header(guid_from_text(first_chunk_guid));
	auto [mesh_res, mesh_geometry] = skinned_mesh_fixture(guid_from_text(mesh_chunk_guid));
	try {
		std::vector<std::byte> archive;
		std::vector<fb::BundleFileInfo> files;
		const auto append_payload = [&](const std::vector<std::byte>& payload) {
			const auto [offset, size] = append_encoded(archive, payload);
			files.push_back({{true, 1, 1}, offset, size});
		};
		append_payload(collection_ebx);
		append_payload(card_ebx);
		append_payload(cdn_ebx);
		append_payload(rime_ebx);
		append_payload(texture_ebx);
		append_payload(resolution_ebx);
		append_payload(rime_res);
		append_payload(texture_res);
		append_payload(resolution_res);
		append_payload(first_chunk);
		append_payload(second_chunk);
		append_payload(mesh_res);
		append_payload(mesh_geometry);
		const fb::CasIdentifier cas{true, 1, 1};

		fb::BinaryBundle manifest;
		const auto add_ebx = [&](std::string asset_name, const std::vector<std::byte>& payload, std::byte hash) {
			fb::BundleAsset asset;
			asset.kind = fb::AssetKind::ebx;
			asset.name = std::move(asset_name);
			asset.sha1.bytes[0] = hash;
			asset.originalSize = payload.size();
			manifest.ebx.push_back(std::move(asset));
		};
		add_ebx("items/itemmastercollection", game_collection, std::byte{0x11});
		add_ebx("items/playercard_icon/own_test_pcard_icon", card_ebx, std::byte{0x12});
		add_ebx(icon_root + "_cdn", cdn_ebx, std::byte{0x13});
		add_ebx(icon_root + "_rime", rime_ebx, std::byte{0x14});
		add_ebx(icon_root + "_texture", texture_ebx, std::byte{0x15});
		add_ebx(icon_root + "_resolution_1_texture", resolution_ebx, std::byte{0x16});
		const auto add_resource = [&](std::string asset_name, const std::vector<std::byte>& payload, std::byte hash) {
			fb::BundleAsset asset;
			asset.kind = fb::AssetKind::resource;
			asset.name = std::move(asset_name);
			asset.sha1.bytes[0] = hash;
			asset.resourceMeta.assign(16, std::byte{0});
			asset.originalSize = payload.size();
			manifest.resources.push_back(std::move(asset));
		};
		add_resource(icon_root + "_rime", rime_res, std::byte{0x21});
		add_resource(icon_root + "_texture", texture_res, std::byte{0x22});
		add_resource(icon_root + "_resolution_1_texture", resolution_res, std::byte{0x23});
		manifest.resources[1].resourceId = 0x12345678;
		manifest.resources[1].resourceType = fb::texture_resource_type;
		manifest.resources[1].resourceMeta[0] = std::byte{11};
		manifest.resources.resize(4);
		manifest.resources[3].kind = fb::AssetKind::resource;
		manifest.resources[3].name = "items/playercard_icon/own_test_mesh";
		manifest.resources[3].sha1.bytes[0] = std::byte{0x24};
		manifest.resources[3].resourceId = 0x23456789;
		manifest.resources[3].resourceType = 0x49b156d4;
		manifest.resources[3].resourceMeta.assign(16, std::byte{0});
		manifest.resources[3].originalSize = mesh_res.size();
		const auto add_chunk = [&](std::string_view guid, const std::vector<std::byte>& payload) {
			fb::BundleAsset asset;
			asset.kind = fb::AssetKind::chunk;
			asset.guid = guid_from_text(guid);
			asset.originalSize = payload.size();
			manifest.chunks.push_back(std::move(asset));
		};
		add_chunk(first_chunk_guid, first_chunk);
		add_chunk(second_chunk_guid, second_chunk);
		add_chunk(mesh_chunk_guid, mesh_geometry);
		check(manifest.ebx.size() == 6 && manifest.resources.size() == 4 && manifest.chunks.size() == 3 &&
			  files.size() == 13, "fixture contains all expected manifest entries and payloads");
		const std::vector<std::string> expected_ebx_assets{
			"items/itemmastercollection", "items/playercard_icon/own_test_pcard_icon",
			icon_root + "_cdn", icon_root + "_rime", icon_root + "_texture", icon_root + "_resolution_1_texture"};
		const std::vector<std::string> expected_resource_assets{
			icon_root + "_rime", icon_root + "_texture", icon_root + "_resolution_1_texture"};
		for (std::size_t index = 0; index < expected_ebx_assets.size(); ++index)
			check(manifest.ebx[index].name == expected_ebx_assets[index], "fixture EBX manifest contains the expected asset path");
		for (std::size_t index = 0; index < expected_resource_assets.size(); ++index)
			check(manifest.resources[index].name == expected_resource_assets[index], "fixture RES manifest contains the expected asset path");
		check(manifest.chunks[0].guid.string() == first_chunk_guid && manifest.chunks[1].guid.string() == second_chunk_guid,
			  "fixture chunk manifest contains the two expected GUIDs");
		const auto region = fb::write_bundle_region(files, fb::write_binary_bundle(manifest));
		const std::vector<fb::TocBundle> toc_bundles{{"items/playercard_icon_bundle", region, 1}};
		const auto toc_file = fb::write_patch_toc(toc_bundles);

		const auto chunk_name = db::make_named_record("name", "pkg").children.front();
		const auto package_index = leaf(8, "persistentIndex", {1, 0, 0, 0});
		const auto install_chunk = container(2, "", {chunk_name, package_index});
		const auto layout_tree = db::write(container(2, "", {
			container(1, "superBundles", {db::make_named_record("name", "Win32/fixture"),
										   db::make_named_record("name", "Win32/not-shipped")}),
			container(2, "installManifest", {container(1, "installChunks", {install_chunk})})}));
		std::vector<unsigned char> layout(db::envelope_size + layout_tree.size());
		std::memcpy(layout.data(), db::magic, sizeof(db::magic));
		std::memcpy(layout.data() + db::envelope_size, layout_tree.data(), layout_tree.size());
		const auto game_chunk = container(2, "", {
			db::make_named_record("name", "gamepkg").children.front(),
			leaf(8, "persistentIndex", {1, 0, 0, 0})});
		const auto game_layout_tree = db::write(container(2, "", {
			container(1, "superBundles", {db::make_named_record("name", "Win32/fixture")}),
			container(2, "installManifest", {container(1, "installChunks", {game_chunk})})}));
		std::vector<unsigned char> game_layout(db::envelope_size + game_layout_tree.size());
		std::memcpy(game_layout.data(), db::magic, sizeof(db::magic));
		std::memcpy(game_layout.data() + db::envelope_size, game_layout_tree.data(), game_layout_tree.size());
		const auto data_chunk = container(2, "", {
			db::make_named_record("name", "pkg").children.front(),
			leaf(8, "persistentIndex", {1, 0, 0, 0})});
		const auto mod_layout_tree = db::write(container(2, "", {
			container(1, "superBundles", {db::make_named_record("name", "Win32/fixture")}),
			container(2, "installManifest", {container(1, "installChunks", {data_chunk})})}));
		std::vector<unsigned char> mod_layout(db::envelope_size + mod_layout_tree.size());
		std::memcpy(mod_layout.data(), db::magic, sizeof(db::magic));
		std::memcpy(mod_layout.data() + db::envelope_size, mod_layout_tree.data(), mod_layout_tree.size());

		write_bytes(mod / L"layout.toc", mod_layout);
		write_bytes(mod / L"Win32" / L"fixture.toc", toc_file);
		write_bytes(mod / L"Win32" / L"pkg" / L"cas_01.cas", archive);
		const auto written_toc = fb::read_toc(read_bytes(mod / L"Win32" / L"fixture.toc"));
		check(written_toc.bundles.size() == 1, "written mod TOC contains the selected test bundle");
		const auto written_region = fb::read_bundle_region(written_toc.bundles.front().region);
		const auto written_manifest = fb::read_binary_bundle(written_region.inlineManifest);
		check(written_region.files.size() == 13 && written_manifest.ebx.size() == 6 &&
			  written_manifest.resources.size() == 4 && written_manifest.chunks.size() == 3,
			  "written mod TOC resolves to all expected assets and payload references");
		for (std::size_t index = 0; index < expected_ebx_assets.size(); ++index)
			check(written_manifest.ebx[index].name == expected_ebx_assets[index], "written mod contains expected EBX asset path");
		for (std::size_t index = 0; index < expected_resource_assets.size(); ++index)
			check(written_manifest.resources[index].name == expected_resource_assets[index], "written mod contains expected RES asset path");
		check(written_manifest.chunks[0].guid.string() == first_chunk_guid &&
			  written_manifest.chunks[1].guid.string() == second_chunk_guid &&
			  written_manifest.chunks[2].guid.string() == mesh_chunk_guid,
			  "written mod contains all expected chunk GUIDs");
		const std::vector<std::vector<std::byte>> expected_payloads{
			collection_ebx, card_ebx, cdn_ebx, rime_ebx, texture_ebx, resolution_ebx,
			rime_res, texture_res, resolution_res, mesh_res, first_chunk, second_chunk, mesh_geometry};
		const auto written_archive = read_bytes(mod / L"Win32" / L"pkg" / L"cas_01.cas");
		for (std::size_t index = 0; index < written_region.files.size(); ++index) {
			const auto& file = written_region.files[index];
			check(file.offset <= written_archive.size() && file.size <= written_archive.size() - file.offset,
				  "written mod payload reference is within its CAS archive");
			const auto raw = std::span(written_archive).subspan(file.offset, file.size);
			check(fb::decode_cas(raw, {.gameRoot = fixture_game}) == expected_payloads[index],
				  "written mod CAS payload bytes match expected asset");
		}
		std::cout << "Verified mod input: 6 EBX, 4 RES, 3 chunks; decoded all 13 CAS payloads.\n";
		write_bytes(fixture_game / L"Data" / L"layout.toc", game_layout);
		fb::BundleAsset base_collection;
		base_collection.kind = fb::AssetKind::ebx;
		base_collection.name = "items/itemmastercollection";
		base_collection.sha1.bytes[0] = std::byte{0x11};
		base_collection.originalSize = game_collection.size();
		fb::BundleAsset base_card;
		base_card.kind = fb::AssetKind::ebx;
		base_card.name = "items/playercard_icon/own_test_pcard_icon";
		base_card.sha1.bytes[0] = std::byte{0x01};
		const fb::BinaryBundle base_manifest{{base_collection, base_card}, {}, {}, {}};
		const std::vector<fb::TocBundle> base_bundles{{"items/playercard_icon_bundle",
			fb::write_bundle_region({}, fb::write_binary_bundle(base_manifest)), 1}};
		write_bytes(fixture_game / L"Data" / L"Win32" / L"fixture.toc", fb::write_patch_toc(base_bundles));
		{
			std::ofstream metadata(mod / L"description.txt", std::ios::binary);
			metadata << "fixture mod";
		}
		const auto sibling_mod = selected_parent / L"CosmeticsPlus";
		fs::copy(mod, sibling_mod, fs::copy_options::recursive);
		const auto sibling_card = ebx_with_texture_reference(0x87654321);
		const auto sibling_payload = fb::encode_cas(sibling_card, {.compression = fb::CasCompression::raw});
		auto sibling_archive = archive;
		check(sibling_payload.size() == files[1].size, "sibling collision fixture uses same-sized CAS payload");
		std::copy(sibling_payload.begin(), sibling_payload.end(), sibling_archive.begin() + files[1].offset);
		write_bytes(sibling_mod / L"Win32" / L"pkg" / L"cas_01.cas", sibling_archive);

		std::vector<std::string> statuses;
		const auto report = mods::dump_mod(mod, selected_output, [&](std::string_view status) {
			statuses.emplace_back(status);
		}, fixture_game);
		check(report.directory == selected_output, "the selected destination is used as the mod-named folder");
		check(report.bundles == 0 && report.ebx == 5 && report.resources == 1 && report.chunks == 0 &&
			report.meshes == 1 && report.loose_files == 0,
			  "five changed EBXs, the referenced texture PNG and the referenced MeshSet GLB are exported");
		const std::vector<fs::path> expected_files{
			"items/playercard_icon/own_test_pcard_icon.ebx",
			icon_root + "_cdn.ebx",
			icon_root + "_rime.ebx",
			icon_root + "_texture.ebx",
			icon_root + "_resolution_1_texture.ebx"};
		std::vector<fs::path> actual_files;
		for (const auto& entry : fs::recursive_directory_iterator(selected_output))
			if (entry.is_regular_file() && entry.path().extension() == L".ebx")
				actual_files.push_back(fs::relative(entry.path(), selected_output));
		std::sort(actual_files.begin(), actual_files.end());
		auto sorted_expected = expected_files;
		std::sort(sorted_expected.begin(), sorted_expected.end());
		if (actual_files != sorted_expected) {
			std::string details = "dump file list mismatch; expected:";
			for (const auto& path : sorted_expected) details += " " + path.generic_string();
			details += "; actual:";
			for (const auto& path : actual_files) details += " " + path.generic_string();
			throw std::runtime_error(details);
		}
		check_file_bytes(selected_output, "items/playercard_icon/own_test_pcard_icon.ebx", card_ebx);
		const auto texture_png = read_bytes(selected_output / L"items" / L"playercard_icon" /
			L"own_test_pcard_icon__own_test_pcard_icon_texture.png");
		const std::array<std::byte, 8> png_signature{std::byte{0x89}, std::byte{'P'}, std::byte{'N'}, std::byte{'G'},
			std::byte{0x0D}, std::byte{0x0A}, std::byte{0x1A}, std::byte{0x0A}};
		check(texture_png.size() > png_signature.size() &&
			  std::equal(png_signature.begin(), png_signature.end(), texture_png.begin()),
			  "the referenced Frostbite texture exports as a PNG beside its EBX");
		const auto mesh_glb = read_bytes(selected_output / L"items" / L"playercard_icon" /
			L"own_test_pcard_icon__own_test_mesh_lod0.glb");
		const std::array<std::byte, 4> glb_signature{std::byte{'g'}, std::byte{'l'}, std::byte{'T'}, std::byte{'F'}};
		check(mesh_glb.size() > 28 && std::equal(glb_signature.begin(), glb_signature.end(), mesh_glb.begin()) &&
			  std::to_integer<std::uint8_t>(mesh_glb[4]) == 2,
			  "the referenced skinned MeshSet exports as a GLB 2.0 mesh beside its EBX");
		const auto json_length = std::to_integer<std::uint32_t>(mesh_glb[12]) |
			(std::to_integer<std::uint32_t>(mesh_glb[13]) << 8) | (std::to_integer<std::uint32_t>(mesh_glb[14]) << 16) |
			(std::to_integer<std::uint32_t>(mesh_glb[15]) << 24);
		const std::string mesh_json(reinterpret_cast<const char*>(mesh_glb.data() + 20), json_length);
		check(mesh_json.find("TEXCOORD_0") != std::string::npos && mesh_json.find("\"indices\":2") != std::string::npos,
			  "the exported GLB declares UV0 and points at the updated index accessor");
		check_file_bytes(selected_output, icon_root + "_cdn.ebx", cdn_ebx);
		check_file_bytes(selected_output, icon_root + "_rime.ebx", rime_ebx);
		check_file_bytes(selected_output, icon_root + "_texture.ebx", texture_ebx);
		check_file_bytes(selected_output, icon_root + "_resolution_1_texture.ebx", resolution_ebx);
		check(!fs::exists(selected_output / L"items" / L"itemmastercollection.ebx"),
			  "the unchanged item collection target is not dumped");
		const auto merge_targets = read_bytes(selected_output / L"merge_targets.txt");
		const std::string merge_targets_text(reinterpret_cast<const char*>(merge_targets.data()), merge_targets.size());
		check(merge_targets_text.find(name + " [Unknown] REPLACES items/playercard_icon/own_test_pcard_icon -> Win32/fixture.toc :: items/playercard_icon_bundle") != std::string::npos,
			  "merge target report identifies same-name game assets as replacements");
		check(merge_targets_text.find(name + " [Unknown] ADDS thumbnail/pcard/reskate/playercard_icon/own_test_pcard_icon_cdn -> Win32/fixture.toc :: items/playercard_icon_bundle") != std::string::npos,
			  "merge target report identifies different-name assets as additions");
		check(
			  merge_targets_text.find("itemmastercollection") == std::string::npos,
			  "merge target report excludes unchanged game assets");
		std::cout << "Verified exact dump contents: " << actual_files.size()
			<< " changed player-card EBX files; itemmastercollection, three RES files, and two chunks omitted.\n";
		check(std::find(statuses.begin(), statuses.end(), "Reading layout.toc") != statuses.end() &&
			  std::find(statuses.begin(), statuses.end(), "Dump complete") != statuses.end(),
			  "progress callbacks report reading and completion stages");
		const auto selected_source = "Dumping selected mod from " + path_utf8(mod);
		check(std::find(statuses.begin(), statuses.end(), selected_source) != statuses.end(),
			  "dump reports the exact selected mod source directory");
		auto selected_archive = path_utf8(mod / L"Win32" / L"pkg" / L"cas_01.cas");
		std::replace(selected_archive.begin(), selected_archive.end(), '\\', '/');
		const auto archive_reported = std::any_of(statuses.begin(), statuses.end(), [&](const auto& status) {
			auto normalized_status = status;
			std::replace(normalized_status.begin(), normalized_status.end(), '\\', '/');
			return normalized_status.find(selected_archive) != std::string::npos;
		});
		if (!archive_reported) {
			std::string details = "selected CAS path not reported: " + selected_archive + "; statuses:";
			for (const auto& status : statuses) details += " [" + status + "]";
			throw std::runtime_error(details);
		}
		bool existing_output_rejected{};
		try { (void)mods::dump_mod(mod, selected_output); }
		catch (const std::runtime_error&) { existing_output_rejected = true; }
		check(existing_output_rejected && fs::exists(selected_output / fs::path(icon_root + "_resolution_1_texture.ebx")),
			  "an existing user-selected dump is not overwritten");

		bool invalid_name_rejected{};
		try { (void)mods::dump_mod("../outside"); }
		catch (const std::invalid_argument&) { invalid_name_rejected = true; }
		check(invalid_name_rejected, "a traversal mod name is rejected");
	} catch (...) {
		fs::remove_all(mod, error);
		fs::remove_all(output, error);
		fs::remove_all(selected_parent, error);
		throw;
	}
	fs::remove_all(mod, error);
	fs::remove_all(output, error);
	fs::remove_all(selected_parent, error);
}
}

int main(int argc, char** argv) {
	if (argc == 5 && std::string_view(argv[1]) == "--dump") {
		try {
			const auto report = mods::dump_mod(argv[2], argv[3], [](std::string_view status) {
				std::cout << status << '\n';
			}, argv[4]);
			std::cout << "Dumped " << report.bundles << " bundles, " << report.ebx << " EBX, "
				<< report.resources << " resources, " << report.chunks << " chunks, and "
				<< report.loose_files << " source files to " << report.directory.string() << '\n';
			return 0;
		} catch (const std::exception& error) {
			std::cerr << "FAILED: " << error.what() << '\n';
			return 1;
		}
	}
	try {
		the_mod_contents_dump();
	} catch (const std::exception& error) {
		std::cerr << "FAILED: " << error.what() << '\n';
		return 1;
	}
	std::cout << "Mod dump tests passed.\n";
}
