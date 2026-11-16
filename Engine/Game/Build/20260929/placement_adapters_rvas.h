#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::placement_adapters {
inline constexpr std::uintptr_t root_flags = 8;
inline constexpr std::uintptr_t grind_context = 8;
inline constexpr std::array<std::uintptr_t, 11> transform_calls{0x7c1cc8, 0x7c65c3, 0x7c9fa1, 0x7cb1dd, 0x7cbc73, 0x7cef58,
                                                                0x7cf547, 0x7d0785, 0x7d0c61, 0x7d0f30, 0x7d161c};
inline constexpr std::array<std::uintptr_t, 11> moving_calls{0xfb6cc6,  0xfb7534,  0xfc1cfc,  0xfeb7f9,  0x1027c6d, 0x1027c7c,
                                                             0x1044140, 0x1044ce9, 0x1045784, 0x10457a4, 0x105709d};
inline constexpr std::array<std::uintptr_t, 2> material_calls{0x102bcf5, 0x102bd47};

}
