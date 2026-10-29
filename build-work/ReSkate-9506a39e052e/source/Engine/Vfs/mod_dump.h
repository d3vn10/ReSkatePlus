#pragma once

#include <cstddef>
#include <filesystem>
#include <functional>
#include <string_view>

namespace dingosdk::mods {

	struct ModDumpReport {
		std::filesystem::path directory;
		std::size_t bundles{}, ebx{}, resources{}, chunks{}, meshes{}, loose_files{};
	};

	using ModDumpProgress = std::function<void(std::string_view)>;

	// Exports a mod from <executable directory>/Mods/<name> to
	// Mods/.reskate/dumps/<name>. Existing contents of that dedicated dump folder
	// are replaced. CAS payloads are decoded into assets/ and non-CAS source files
	// are retained under source/.
	[[nodiscard]] ModDumpReport dump_mod(std::string_view name);

	// Exports an installed mod from its directory into a new output directory.
	// The output directory must not exist and must not be inside the mod folder.
	// Progress messages are short status updates suitable for display in a UI.
	[[nodiscard]] ModDumpReport dump_mod(const std::filesystem::path& mod_directory, const std::filesystem::path& output_directory,
		const ModDumpProgress& progress = {}, const std::filesystem::path& game_root = {});

}
