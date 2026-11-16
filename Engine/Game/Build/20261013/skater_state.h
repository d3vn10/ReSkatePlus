#pragma once
#include "client_source_spawn.h"
#include <array>
#include <cstdint>
#include "skater_state_rvas.h"

namespace dingosdk::game::build::v20261013::skater_state {
// Supported SHA-256 fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9.
// The skater's live state, read-only (Extension/Skater/skater_physics.h).

// The physics state the core's selector chose this step (no_bail.h: choose_physics_state; its
// offboard_physics_state is the one on foot and through a whole bail).
// The length of the step being simulated, seconds (copied from the core's step input every update; skater_body.h).
inline constexpr std::array<std::uint32_t, 2> board_air_states{200, 201}; // PHYSICS_AIR, KNOWN_AIR

// The core's trick state (its vtable slot +0x00 returns core+0x000) holds the offboard state
// at +0x00: the skater on foot and in a ragdoll (the script native GetOffboardScoring copies
// it out). Each offboard substate class raises exactly one substate flag of it from its vtable
// (0x000000000 on, each beside its name and id); the three ragdoll ones, FollowRagdoll (3)
// 0x000000000, FollowAnimatedRagdoll (4) 0x000000000 and FollowSimulatedRagdoll (5) 0x000000000,
// the same one. In the air is the substate's own +0x000, copied over. Flags are one byte, 0 or 1.

// How fast the skater moves: the linear velocity of its physics bodies, read as the SDK's noclip
// reads and writes them (Extension/Skater/client_noclip.cpp). The board's physics: core+0x000 ->
// +0x00 (vtable board_physics_vtable); the skeleton's: the rig (core+0x000) -> +0x0000 (vtable
// rig_physics_vtable). Each keeps its bodies at +0x00: their count (uint32) at the start, body n at
// n * 0x000, each pointing back to its owner at +0x00 with its velocity (float[3], m/s) at +0x00 and
// its spin (angular velocity, float[3], radians per second) at +0x00. 0x000000000 syncs a body with
// the physics world: by its pending flags (+0x00) it sets the velocity (8, 0x000000000) from +0x00
// and the spin (0x00, 0x000000000) from +0x00. Flag 1 sets rotation and position from the world matrix
// at +0x00 (0x000000000); flag 4 sets the position from +0x00, keeping the rotation the physics has
// (0x000000000). Read in the disassembly, 2026-11-06.
// The board has 9, body 0 its root; the skeleton 26, body n the ragdoll's body n (skater_body.h).
// core + core_steps_since_built (uint32) counts the core's physics steps from its last build, at a spawn
// or a teleport: one a step in every run compared on 2026-11-06. The core keeps its address across a
// teleport, so the address does not show one.
// A sensor block in the skater's context (+0x0000 to +0x0000), found by ReTake's state dumps on 2026-11-08: in a
// skater ridden since the game started, +0x0000 and +0x0000 are exactly 1.0 at every step (bails and all), +0x0000
// 0.7, +0x0000 0 and +0x0000 (float[3]) 0, and +0x0000 on holds a direction that follows the skater. In a skater only
// placed by a replay since the game started they stay 0, 0, 0, FLT_MAX and an up normal, the direction never comes,
// and the replay takes a rail the other way (context+0x000 -1 where the ride had 1). What they mean is not read yet.
inline constexpr std::uintptr_t context_sensor_on_a = 0x0000, context_sensor_on_b = 0x0000; // float, 1 started
inline constexpr std::uint32_t body_flag_rotation = 1, body_flag_position = 4, body_flag_velocity = 8, body_flag_spin = 0x00;
// A flight on the board (no_bail.cpp, 2.7): the air state (core+0x000, its launch block at +0x00, the core's at core+0x000)
// counts its frames at +0x000, one a step (0x48256b0), and reads the block's path at t = frame * dt (0x474ea20). The block's
// dt (+0x0000) is set when the skater is built (0x4753f94), so it does not follow a step changed since.
// The block's current jump request (+0x0000, 0x4750ea0) has a frame time of its own, read at frame * dt by the air state
// for its velocity and placing (0x4818ec5, 0x481e8cb, 0x4824981), and the flight's length in frames: every step the
// air state takes frame / frames as how far along it is (0x4825831).
inline constexpr std::uint32_t board_body_count = 9, rig_body_count = 26;
// A body's pending flags (uint32 at +0x00): what the sync above pushes into the physics world next, 8 the
// velocity, 0x00 the spin (noclip, the push hold and the dive boost set them beside their writes).
inline constexpr std::uint32_t body_pending_velocity = 8, body_pending_spin = 0x00;
// The off-board state machine (Engine/Game/Build/20260929/offboard_flight.h) hangs off the core at +0x000; its
// substate running now at +0x00, a pointer into the machine itself, so the substate is known by its offset there
// (offboard_flight.h's offboard_ragdoll_states and offboard_flight_states list them).
inline constexpr std::size_t board_root_body = 0;
inline constexpr std::uintptr_t board_physics_vtable = client_source_spawn::board_physics_vtable;
inline constexpr std::uintptr_t rig_physics_vtable = client_source_spawn::rig_physics_vtable;
inline constexpr std::uintptr_t physics_state_offset = 0x0000; // uint32, context
inline constexpr std::uintptr_t step_seconds_offset = 0x0000; // float, context
inline constexpr std::uintptr_t trick_state_offset = 0x000;   // core
inline constexpr std::uintptr_t offboard_state_offset = 0x00; // trick state
inline constexpr std::uintptr_t ragdoll_offset = 0x000;       // offboard state
inline constexpr std::uintptr_t in_the_air_offset = 0x000;    // offboard state
inline constexpr std::uintptr_t core_steps_since_built = 0x000;
inline constexpr std::uintptr_t context_sensor_far = 0x0000;    // float: 0 started, FLT_MAX not
inline constexpr std::uintptr_t context_sensor_normal = 0x0000; // float[3]: 0 started, an up normal not
inline constexpr std::uintptr_t context_sensor_gain = 0x0000;   // float: 0.7 started, 0 not
inline constexpr std::uintptr_t body_flags_offset = 0x00;
inline constexpr std::uintptr_t core_jump_block_offset = 0x000;
inline constexpr std::uintptr_t core_air_state_offset = 0x000;
inline constexpr std::uintptr_t air_state_block_offset = 0x00;
inline constexpr std::uintptr_t air_state_frame_offset = 0x000; // int32
inline constexpr std::uintptr_t jump_block_dt_offset = 0x0000;  // float, seconds
inline constexpr std::uintptr_t jump_block_request_offset = 0x0000;
inline constexpr std::uintptr_t request_frames_offset = 0x00;  // int32
inline constexpr std::uintptr_t request_dt_offset = 0x00;      // float, seconds
inline constexpr std::uintptr_t board_holder_offset = 0x000;  // core
inline constexpr std::uintptr_t board_physics_offset = 0x00;  // board holder
inline constexpr std::uintptr_t rig_physics_offset = 0x0000;  // rig
inline constexpr std::uintptr_t physics_bodies_offset = 0x00; // board or rig physics
inline constexpr std::uintptr_t physics_body_size = 0x000;
inline constexpr std::uintptr_t body_owner_offset = 0x00;
inline constexpr std::uintptr_t body_velocity_offset = 0x00;
inline constexpr std::uintptr_t body_spin_offset = 0x00;
inline constexpr std::uintptr_t body_pending_offset = 0x00;
inline constexpr std::uintptr_t offboard_machine_offset = 0x000; // core
inline constexpr std::uintptr_t offboard_active_offset = 0x00;   // machine

}
