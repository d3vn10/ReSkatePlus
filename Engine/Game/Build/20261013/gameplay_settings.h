#pragma once
#include <array>
#include <cstdint>
#include "gameplay_settings_rvas.h"

namespace dingosdk::game::build::v20261013::gameplay_settings {
// Dedicated fast-travel getter: independent evidence for the
// FastTravelPointsEnabled byte, and its expected first bytes.
inline constexpr std::array<unsigned char, 32> fast_travel_getter_prefix{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

// Settings sections: TypeInfo handle (settings lookup key) and vtable.
// DelMarGameSettings is engine::game_settings_handle / engine::game_settings_vtable.

// Reflection member records ({name hash, offset, type}) of the overridden
// boolean fields, named after the field.
// DingoActivitySettings
// DingoAdvanceSettings
// PlayerProgressionSystemSettings
// DelMarGameSettings
// DelMarUISettings
// DingoProgressionSettings
}
