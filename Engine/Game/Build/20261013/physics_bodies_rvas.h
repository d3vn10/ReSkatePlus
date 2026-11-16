#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::physics_bodies {
inline constexpr std::uintptr_t body_bit_a70_set = 0x25d38a0;
inline constexpr std::uintptr_t body_bit_a70_clear = 0x25d3250;
inline constexpr std::uintptr_t body_bit_f88_set = 0x25d3930;
inline constexpr std::uintptr_t body_bit_4c0_set = 0x25e51a0;
inline constexpr std::uintptr_t body_bit_4c0_clear = 0x25e4c90;
inline constexpr std::uintptr_t body_bit_310_set = 0x25d3560;
inline constexpr std::uintptr_t body_bit_310_clear = 0x25e4e60;
inline constexpr std::uintptr_t body_bit_830_set = 0x25e5360;
inline constexpr std::uintptr_t body_bit_830_clear = 0x25e4e90;
inline constexpr std::uintptr_t body_bit_8d0_set = 0x25e5470;
inline constexpr std::uintptr_t body_bit_8d8_set = 0x25e56d0;
inline constexpr std::uintptr_t body_bit_aa0_set = 0x25e5440;
inline constexpr std::uintptr_t body_bit_aa0_clear = 0x25e4fa0;
inline constexpr std::uintptr_t body_bit_1598_set = 0x25e51d0;
inline constexpr std::uintptr_t body_bit_40_set = 0x25e5100;
inline constexpr std::uintptr_t body_bit_40_clear = 0x25e4c30;
inline constexpr std::uintptr_t body_bit_1680_set = 0x25e5300;

inline constexpr std::uintptr_t ghost_body = 0x25d3610;
inline constexpr std::uintptr_t solid_body = 0x25d3170;
inline constexpr std::uintptr_t main_ghost = 0x25d3360;
inline constexpr std::uintptr_t main_solid = 0x25d2e90;
}
