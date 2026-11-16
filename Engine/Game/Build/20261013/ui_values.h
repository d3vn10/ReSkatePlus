#pragma once
#include <array>
#include <cstdint>
#include "ui_values_rvas.h"

namespace dingosdk::game::build::v20261013::ui_values {
// The asset loader publishes a symbolic alias separately from the typed model
// handle. The insertion routine holds no lock; callers hold ModelWriteLock.
inline constexpr std::array<unsigned char, 32> alias_insert_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::uintptr_t models_offset = 0x000;
inline constexpr std::uintptr_t aliases_offset = 0x000;
inline constexpr std::uintptr_t alias_value_offset = 8;

}
