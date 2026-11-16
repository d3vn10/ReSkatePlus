#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::placement_physics {
inline constexpr std::uintptr_t body_pose = 0x25d6720;
inline constexpr std::uintptr_t sleep = 0x25ee3d0;
inline constexpr std::uintptr_t motion_type = 0x25e79c0;

inline constexpr std::uintptr_t marker_save = 0x544640;
inline constexpr std::uintptr_t marker_return = 0x53e7a0;
}
