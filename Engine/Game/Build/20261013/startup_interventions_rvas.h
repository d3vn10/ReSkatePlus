#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::startup_interventions {
inline constexpr std::uintptr_t frame = 0x00000000;
inline constexpr std::uintptr_t frame_data = 0x00000000; // R11: clear bytecode
inline constexpr std::uintptr_t frame_cursor = 0x00000000; // [RCX + 0x00]
inline constexpr std::uintptr_t exit_thunk = 0x00000000;
inline constexpr std::uintptr_t exit_return = 0x00000000;

}
