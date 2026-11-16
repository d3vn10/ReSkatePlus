#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::skater_body {
inline constexpr std::uintptr_t core_step_input_offset = 0x3c8;   // core
inline constexpr std::uintptr_t step_input_length_offset = 0x20; // step input, float seconds
inline constexpr std::uintptr_t contact_holder_offset = 0x2f10; // rig
inline constexpr std::uintptr_t contact_struct_offset = 0x1040; // holder
inline constexpr std::uintptr_t bone_records_offset = 0x5e0;     // contact struct
inline constexpr std::uintptr_t bone_record_size = 0x70;
inline constexpr std::uintptr_t bone_peak_offset = 0x50;      // float, ordinary contacts
inline constexpr std::uintptr_t bone_tracked_peak_offset = 0x54; // float, near a tracked point
inline constexpr std::uintptr_t body_transform_offset = 0x20;
inline constexpr std::uintptr_t bone_slide_offset = 0x20;        // float[4], and its tracked one at +0x30
inline constexpr std::uintptr_t bone_tracked_offset = 0x10;      // from an ordinary field to its tracked one
inline constexpr std::uintptr_t bone_hit_vehicle_offset = 0x60;  // bytes
inline constexpr std::uintptr_t bone_hit_kind_11_offset = 0x61;
inline constexpr std::uintptr_t bone_hit_kind_5_offset = 0x62;
inline constexpr std::uintptr_t bone_hit_world_offset = 0x63;
inline constexpr std::uintptr_t bone_hit_board_offset = 0x64;
inline constexpr std::uintptr_t bone_touching_offset = 0x340;    // contact struct, byte per bone

}
