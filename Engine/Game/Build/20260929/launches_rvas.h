#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::launches {
inline constexpr std::uintptr_t set_trajectory = 0x4752aa0;
inline constexpr std::uintptr_t query_manager = 0x7785da8;
inline constexpr std::uintptr_t query_manager_load = 0x473e312;
inline constexpr std::uintptr_t foot_gravity = 0x4834a95;
inline constexpr std::uintptr_t foot_speed_cap = 0x65e8238;
inline constexpr std::uintptr_t stance_flags_offset = 0x13c4; // context

}
