#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::skater_render {
inline constexpr std::uintptr_t render_object_vtable = 0x65f2910;
inline constexpr std::uintptr_t render_index_offset = 0x108; // render object, uint32
inline constexpr std::uintptr_t render_object_lock_offset = 0x104; // render object, uint32: set while packing
inline constexpr std::uintptr_t packet_actor_offset = 0x00;     // float[16], copied from object+0x50
inline constexpr std::uintptr_t packet_bones_offset = 0x80;     // pointer to float[12] per bone (object+0xe0)
inline constexpr std::uintptr_t packet_bone_count_offset = 0x88; // uint32 (object+0xf8)
inline constexpr std::uintptr_t packet_root_offset = 0x90;      // pointer to float[4] (object+0xd8)
inline constexpr std::uintptr_t packet_relative_offset = 0x98;  // byte (object+0xfe)
inline constexpr std::uintptr_t packet_visible_offset = 0x99;   // byte (object+0xff)
inline constexpr std::uintptr_t entity_render_handles_offset = 0x4d0; // skater entity, uint32 x 2
inline constexpr std::uintptr_t entity_render_handle_stride = 8;
inline constexpr std::uintptr_t manager_pool_offset = 0xc8;
inline constexpr std::uintptr_t view_camera_offset = 0x40;  // float[16]
inline constexpr std::uintptr_t view_fov_offset = 0x104;    // float, radians
inline constexpr std::uintptr_t view_kind_back = 0x38;      // uint32, before the view

}
