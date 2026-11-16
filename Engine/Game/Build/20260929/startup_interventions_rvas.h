#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::startup_interventions {
inline constexpr std::uintptr_t frame = 0x08ae2785;
inline constexpr std::uintptr_t frame_data = 0x08b7bf3e; // R11: clear bytecode
inline constexpr std::uintptr_t frame_cursor = 0x08b7bf72; // [RCX + 0xd8]
inline constexpr std::uintptr_t exit_thunk = 0x08a06ff3;
inline constexpr std::uintptr_t exit_return = 0x087fe7fb;

}
