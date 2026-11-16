#pragma once
#include <cstdint>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::animation_values {
inline constexpr std::uintptr_t float_type = 0x61ab550;

inline constexpr std::uintptr_t bool_type = 0x61ab030, bool_secondary = 0x61ab080;
inline constexpr std::uintptr_t enum_type = 0x61926d8, enum_secondary = 0x6192728;
}
