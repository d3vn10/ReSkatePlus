#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::skater_state {
inline constexpr std::uintptr_t physics_state_offset = 0x1414; // uint32, context
inline constexpr std::uintptr_t step_seconds_offset = 0x17ec; // float, context
inline constexpr std::uintptr_t trick_state_offset = 0x3b8;   // core
inline constexpr std::uintptr_t offboard_state_offset = 0x78; // trick state
inline constexpr std::uintptr_t ragdoll_offset = 0x18c;       // offboard state
inline constexpr std::uintptr_t in_the_air_offset = 0x180;    // offboard state
inline constexpr std::uintptr_t core_steps_since_built = 0x190;
inline constexpr std::uintptr_t context_sensor_far = 0x1a10;    // float: 0 started, FLT_MAX not
inline constexpr std::uintptr_t context_sensor_normal = 0x1a20; // float[3]: 0 started, an up normal not
inline constexpr std::uintptr_t context_sensor_gain = 0x1b14;   // float: 0.7 started, 0 not
inline constexpr std::uintptr_t body_flags_offset = 0x60;
inline constexpr std::uintptr_t core_jump_block_offset = 0x3e8;
inline constexpr std::uintptr_t core_air_state_offset = 0x308;
inline constexpr std::uintptr_t air_state_block_offset = 0x30;
inline constexpr std::uintptr_t air_state_frame_offset = 0x4dc; // int32
inline constexpr std::uintptr_t jump_block_dt_offset = 0x18b4;  // float, seconds
inline constexpr std::uintptr_t jump_block_request_offset = 0x1100;
inline constexpr std::uintptr_t request_frames_offset = 0x80;  // int32
inline constexpr std::uintptr_t request_dt_offset = 0xe4;      // float, seconds
inline constexpr std::uintptr_t board_holder_offset = 0x430;  // core
inline constexpr std::uintptr_t board_physics_offset = 0x18;  // board holder
inline constexpr std::uintptr_t rig_physics_offset = 0x2f10;  // rig
inline constexpr std::uintptr_t physics_bodies_offset = 0x20; // board or rig physics
inline constexpr std::uintptr_t physics_body_size = 0x130;
inline constexpr std::uintptr_t body_owner_offset = 0x10;
inline constexpr std::uintptr_t body_velocity_offset = 0x70;
inline constexpr std::uintptr_t body_spin_offset = 0x90;
inline constexpr std::uintptr_t body_pending_offset = 0x60;
inline constexpr std::uintptr_t offboard_machine_offset = 0x3b0; // core
inline constexpr std::uintptr_t offboard_active_offset = 0x48;   // machine

}
