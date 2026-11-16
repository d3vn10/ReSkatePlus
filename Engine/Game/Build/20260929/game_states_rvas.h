#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::game_states {
inline constexpr std::uintptr_t state_lookup = 0x11dcdd0;
inline constexpr std::uintptr_t float_setter = 0x11dd7d0;
inline constexpr std::uintptr_t state_write_lookup = 0x11dcff0;
inline constexpr std::uintptr_t state_copy = 0x11d9a40;
inline constexpr std::uintptr_t rate_compute = 0x10dd2f0;

}
