#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <array>
#include <cstdint>
#include "mesh_variations_rvas.h"

// The game's mesh variation databases (Extension/World/mesh_unload_guard.cpp).
namespace dingosdk::game::build::v20260929::mesh_variations {
// remove_databases(registry, ?, list): run on a job worker as a level's bundles unload. For
// each MeshVariationDatabase in the list it takes every entry out of the registry
// (0x3f5a200), keyed by the entry's mesh (its name hash, mesh + 0x00) and variation hashes
// (entry + 0x00, + 0x00). It holds the registry's lock (0x158f2c0 / 0x158f430) throughout.

// The read of the mesh's name hash (mov edx, [rax + 0x00]), with no check that the entry still
// has its mesh: an access violation reading 0x00 when the mesh was unloaded before its
// database (crash reports of 2026-10-08 to 2026-10-11, always on leaving one custom map).
inline constexpr std::array<unsigned char, 6> mesh_hash_read_bytes{0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// The step to the next entry (add rbx, 0x00), after the call that takes one out: where an entry
// without a mesh goes on from.
inline constexpr std::array<unsigned char, 4> next_entry_bytes{0x00, 0x00, 0x00, 0x00};
}
