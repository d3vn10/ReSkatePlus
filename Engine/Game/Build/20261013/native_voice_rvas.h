#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::native_voice {
inline constexpr std::uintptr_t sound_create = 0x13f3b70;
inline constexpr std::uintptr_t sound_destroy = 0x13f3d40;
inline constexpr std::uintptr_t sound_set_input = 0x13f4af0;
inline constexpr std::uintptr_t samples_submit = 0x454350;
inline constexpr std::uintptr_t samples_remove = 0x45c240;
inline constexpr std::uintptr_t sound_manager = 0x72c75c8;
inline constexpr std::uintptr_t sound_manager_vtable = 0x61e1b38;
inline constexpr std::uintptr_t samples_provider = 0x71e3ad8;
inline constexpr std::uintptr_t spatializer = 0x71e3ae0;
inline constexpr std::uintptr_t voice_asset_vtable = 0x61dbdc8;
inline constexpr std::uintptr_t source_input_type = 0x71ff2d0;
inline constexpr std::uintptr_t transform_input_type = 0x72d0ba0;

}
