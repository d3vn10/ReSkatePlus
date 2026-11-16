#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::game_states {
inline constexpr std::uintptr_t state_lookup = 0x11e0ca0;
inline constexpr std::uintptr_t float_setter = 0x11e16a0;
inline constexpr std::uintptr_t state_write_lookup = 0x11e0ec0;
inline constexpr std::uintptr_t state_copy = 0x11dd910;
inline constexpr std::uintptr_t rate_compute = 0x10e1090;

inline constexpr std::uintptr_t ant_driven_vtable = 0x63aaee8;
inline constexpr std::uintptr_t states_block_vtable = 0x61a8bd8;
}
