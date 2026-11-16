#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include "physics_bodies.h"
#include <array>
#include <cstdint>
#include "rigid_bodies_rvas.h"

// Native rigid-body operations and the body/shape layout of the supported game build, for
// Extension/World/owned_bodies.cpp. Callers still qualify their own world and body and compare
// these bytes before any call; a pin here does not qualify an owner, API or command by itself.
namespace dingosdk::game::build::v20260929::rigid_bodies {


// (body, const vec4*) stores the authored m/I principal-axis ratio in state+0x000.

// The game's body synchronizer, which itself produces force and torque inputs through
// 0x25d14b0/0x25d19a0. A matching local wrapper is evidence of that producer phase, not a
// world-wide barrier.

// Overwrite a dynamic body's pending force or torque; parking a body writes zero to both.


// The physics state layout the owned-body service reads. The state is the world's store
// (physics_bodies::store_offset). Every read reroots the current world and repeats the
// allocation, generation and array checks.
inline constexpr std::uintptr_t position_ring = 0x000, body_capacity = 0x000;
inline constexpr std::uintptr_t local_com = 0x000, rotation_ring = 0x000, local_rotation = 0x000;
inline constexpr std::uintptr_t velocity_ring = 0x000, spin_ring = 0x000;
inline constexpr std::uintptr_t pending_force = 0x000, pending_torque = 0x000;
inline constexpr std::uintptr_t inverse_mass_array = 0x000, inertia_ratio_array = 0x000;
inline constexpr std::uintptr_t allocated_bits = 0x000, dynamic_bits = 0x000;
// The native hit reader returns a shape (what physics_bodies.h calls a body). 0x25d3cb0 maps
// that shape through F98; 0x25d7e00 checks its allocation bit and byte generation in this state.
inline constexpr std::uintptr_t shape_allocated_bits = 0x000, shape_body_indices = 0x000, shape_generations = 0x000;
// 0x25d7b10 compares a body's u32 generation in the world, not its state.
inline constexpr std::uintptr_t shape_categories = 0x000, shape_collides = 0x000;
// A shape's owner slot is physics_bodies::body_owner_offset, indexed by the shape. A slot of
// FFFFFFFF has no owner: set_shape_owner stores the owner's u32 index there, -1 for none, and
// the native ignore lists skip their test for it.
inline constexpr std::uint32_t unassigned_shape_owner = 0xffffffffU;
// 0x25d55d0 takes a shape handle and stores DWORD (World+28)+FE8[index]
// through its second argument, with no index, allocation or generation check.
// The SDK never calls it. Its bytes qualify the store_offset/shape_categories
// layout that guarded reads use for the optional in_categories hit field; a
// mismatch disables that field only.
inline constexpr std::array<unsigned char, 32> shape_in_categories_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

// The local client/physics participant qualifies the observed producer phase;
// it does not grant ownership of that participant or its bodies to a plugin.
inline constexpr std::uintptr_t client_context = 8, client_phase = 0x00;
inline constexpr std::uintptr_t tls_active = 0x000, tls_context = 0x000;
inline constexpr std::uintptr_t player_begin = 0x000, player_end = 0x000;
inline constexpr std::uintptr_t player_context = 0x00, player_local = 0x00, player_remote = 0x00;
inline constexpr std::uintptr_t player_entity = 0x00, player_handle = 0x00;
inline constexpr std::uintptr_t entity_context = 0x00, entity_player = 0x00, entity_teleport = 0x000;
inline constexpr std::uintptr_t entity_collection = 0x00, entity_component = 0x000;
// The local player's handle points this far into its skater entity, at a secondary subobject.
inline constexpr std::uintptr_t component_collection = 0x00, component_core = 0x00;
inline constexpr std::uintptr_t core_context = 0x000, core_rig = 0x000, rig_core = 0x0000;
inline constexpr std::uint32_t safe_categories = 0, safe_world_collisions = 0x000;
} // namespace dingosdk::game::build::v20260929::rigid_bodies
