#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::native_audio {
inline constexpr std::uintptr_t submit = 0x44a370;
inline constexpr std::uintptr_t frame_copy = 0x58f03b0;
inline constexpr std::uintptr_t frame_difference = 0x58d53a0;
inline constexpr std::uintptr_t frame_reset = 0x58ced60;
inline constexpr std::uintptr_t stop_sound = 0x415250;

}
