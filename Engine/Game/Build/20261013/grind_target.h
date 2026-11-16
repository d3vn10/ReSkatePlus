#pragma once
#include <array>
#include <cstdint>
#include "grind_target_rvas.h"
namespace dingosdk::game::build::v20261013::grind_target {
// Ordinary airborne rail candidate selection. Native geometry and acceptance tests still run.
inline constexpr std::array<std::uint8_t,16> select_prefix{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0,0,0,0x00,0x00,0x00};
inline constexpr std::uintptr_t context = 0x00, request_gate = 0x000, alignment = 0x000;
inline constexpr std::uintptr_t requested = 0x0000, request_flags = 0x0000;
inline constexpr std::uint32_t requested_mask = 0x00000;
inline constexpr std::uintptr_t root = 0x000, target = 0x00, targeting = 0x000, direction = 0x00;
}
