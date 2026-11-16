#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include "mesh_settings_rvas.h"

// A mesh's render settings (Extension/World/mesh_settings_guard.cpp).
namespace dingosdk::game::build::v20260929::mesh_settings {
// apply_settings(mesh, settings): as a level's meshes load (0x3f59610, for each mesh whose flag
// at + 0x000 is clear), it builds the settings object of the mesh's asset in place at mesh + 0x000
// (0x00 bytes, a reflected instance of 0x76f26f0) when the asset has one; settings is that
// object's source, null when it has none. Its caller sets the flag either way.

// The mesh (0x000 bytes, made at 0x3f33770) leaves + 0x000 to + 0x000 as the allocation had it.
inline constexpr std::ptrdiff_t settings_offset = 0x000;
// The settings object's string the mesh frees as it unloads (0x3f5fa20, through 0x1584c50,
// whenever the flag is set): with no settings object it is whatever the allocation left there.
inline constexpr std::ptrdiff_t unload_string_offset = 0x000;
inline constexpr std::ptrdiff_t applied_flag_offset = 0x000;
// The game's empty string: a string that holds it frees nothing (0x1584c50 compares with it).
}
