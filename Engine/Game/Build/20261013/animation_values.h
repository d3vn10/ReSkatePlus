#pragma once
#include <cstdint>
#include "animation_values_rvas.h"
namespace dingosdk::game::build::v20260929::animation_values {
inline constexpr std::uintptr_t core_position = 0x000, core_forward = 0x000, core_backwards = 0x000;
inline constexpr std::uintptr_t frame_source = 0x000, frame_flags = 0x00;
inline constexpr std::uintptr_t frame_context = 0x0000, basis_context = 0x0000;
inline constexpr std::uint32_t native_mask = 1u << 11, basis_mask = 1u << 13;
inline constexpr std::uintptr_t state_root = 0x000, export_state = 0x00, exported_flags = 0x00;
// Occupied per-rider animation graph properties, build 20260929. The descriptor carries
// its inline name and an encoded byte offset, not an array index. No defaults are used.
inline constexpr std::uintptr_t provider = 0x000, model = 0x000;
inline constexpr std::uintptr_t input = 0x00, output = 0x00, graph_bytes = 0x000;
inline constexpr std::uintptr_t type = 0x00, secondary_type = 0x00, key = 0x00;
inline constexpr std::uintptr_t bool_name = 0x00, float_name = 0x00;
inline constexpr std::uintptr_t pages = 0x00, occupied = 0x000, last_value = 0x000;
}
