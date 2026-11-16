#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::client_source_spawn {
inline constexpr std::uintptr_t source_create = 0x51fef0;
inline constexpr std::uintptr_t context_client_offset = 0x6fc25f0;
inline constexpr std::uintptr_t camera_local_id = 0x73c7ab8;
inline constexpr std::uintptr_t camera_controller_vtable = 0x63efab8;
inline constexpr std::uintptr_t camera_mode_switch = 0x2f2c360;
inline constexpr std::uintptr_t free_camera_vtable = 0x63fc220;
inline constexpr std::uintptr_t camera_transform_setter = 0x2f8fa90;
inline constexpr std::uintptr_t camera_fov_init = 0x2f8bb70;
inline constexpr std::uintptr_t view_manager = 0x75cbc48;
inline constexpr std::uintptr_t view_manager_registration = 0x75dd288;
inline constexpr std::uintptr_t view_manager_vtable = 0x63ece68;
inline constexpr std::uintptr_t camera_view_vtable = 0x63ed0a0;
inline constexpr std::uintptr_t ui_settings = 0x7761aa0;
inline constexpr std::uintptr_t ui_settings_vtable = 0x65ae378;
inline constexpr std::uintptr_t ui_settings_type = 0x77674d0;
inline constexpr std::uintptr_t board_physics_vtable = 0x65e67a0;
inline constexpr std::uintptr_t rig_physics_vtable = 0x65dabc0;
inline constexpr std::uintptr_t physics_update = 0x47ddca0;
inline constexpr std::uintptr_t skater_motion = 0x476fd50;
inline constexpr std::array<std::uintptr_t, 2> free_camera_helper_vtables{0x63fc1e0, 0x63fc200};
inline constexpr std::array<std::uintptr_t, 4> view_manager_methods{0x2f11470, 0x2f0f700, 0x2f0d7c0, 0x2f0d0b0};

}
