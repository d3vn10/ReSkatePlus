#pragma once
#include <array>
#include <cstdint>
#include "gameplay_settings_rvas.h"

namespace dingosdk::game::build::v20260929::gameplay_settings {
// Dedicated fast-travel getter: independent evidence for the
// FastTravelPointsEnabled byte, and its expected first bytes.
inline constexpr std::array<unsigned char, 32> fast_travel_getter_prefix{0x48,0x83,0xec,0x28,0x48,0x8b,0x0d,0x95,0x76,0xc0,0x06,0x48,0x8d,0x15,0xae,0x23,0x9e,0x06,0xe8,0x99,0xb8,0x05,0x01,0x48,0x85,0xc0,0x74,0x0d,0x80,0x78,0x29,0x00};

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
