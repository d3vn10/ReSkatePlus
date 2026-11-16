#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::localization {
inline constexpr std::uintptr_t override_store = 0x331d5e0;
inline constexpr std::uintptr_t override_set = 0x3333770;
inline constexpr std::uintptr_t string_exists = 0x33377c0;
inline constexpr std::uintptr_t database_loaded = 0x761b4a0;
inline constexpr std::uintptr_t databases = 0x761b4a8;
inline constexpr std::uintptr_t database_last = 0x716d524;
inline constexpr std::uintptr_t case_upper_table = 0x682c480;
inline constexpr std::uintptr_t case_lower_table = 0x682cfa0;

inline constexpr game::build::Fingerprint case_convert{0x54a6bc0, {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
}
