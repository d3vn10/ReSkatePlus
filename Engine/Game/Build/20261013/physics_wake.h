#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>

// The physics entity system's wake pass (Extension/World/physics_wake_guard.cpp).
//
// wake(system), a per-frame callback of the Physics entity system (its vtable at 0x64c6ff8,
// called from the entity world's update at 0x3cd6055), collects the bodies changed this step
// (0x269c270 on the world at system + 0x00), finds each one's physics owner (0x25d5f90, by the
// state's body-to-owner list at + 0x000) and calls each owner once (owner + 0x00 -> vtbl + 0x00).
// It keeps "once" in a bitset on the heap with a bit for each of `owner_limit` owners
// (system + 0x00, a Physics setting: 20000), and sets the bit of an owner's index (0x3d2f846)
// without checking it against that. An index is good while it is below the world's owner
// count (its generation list, world + 0x000 .. + 0x000, four bytes each; 0x25d7d90), and the
// owner pool hands indices out first in, first out, so after a long session in which many
// bodies come and go (multiplayer's remote skaters, made and removed as players cross the
// build radius) indices pass 20000. The bit then lands in the next heap block's header, and a
// later free crashes in the allocator (Skate.exe+0x3d99e2f, +0x3d9ee43, +0x3d9ee47; indices
// 20199, 20203 and 20215 in three dumps, the next block's size off by exactly that bit;
// 2026-10-21).
// The owner count is past 20000 in every session because ReSkate raises the owner pool to 160000
// (Extension/World/physics_world_size.cpp) and the generation list is sized to the pool; the setting
// is written only by the system's init (0x3d2fe20) and read only by the wake pass (2026-10-24).
#include "physics_wake_rvas.h"

namespace dingosdk::game::build::v20261013::physics_wake {

inline constexpr std::ptrdiff_t world_offset = 0x00;       // system + 0x00: the world
inline constexpr std::ptrdiff_t owner_limit_offset = 0x00; // system + 0x00: u32, the bitset's size in owners
inline constexpr std::ptrdiff_t owner_generations_begin = 0x000, owner_generations_end = 0x000; // world: u32 each
}
