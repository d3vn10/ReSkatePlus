#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::localization {
inline constexpr std::uintptr_t override_store = 0x331d5e0;
inline constexpr std::uintptr_t override_set = 0x331cc20;
inline constexpr std::uintptr_t string_exists = 0x3320c70;
inline constexpr std::uintptr_t database_loaded = 0x761b4a0;
inline constexpr std::uintptr_t databases = 0x761b4a8;
inline constexpr std::uintptr_t database_last = 0x716d524;
inline constexpr std::uintptr_t case_upper_table = 0x682c480;
inline constexpr std::uintptr_t case_lower_table = 0x682cfa0;

inline constexpr game::build::Fingerprint case_convert{0x54a6bc0, {
    0x40,0x53,0x55,0x56,0x57,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x83,0xec,0x60,0x48,
    0x8b,0x05,0xea,0xd7,0xd1,0x01,0x48,0x33,0xc4,0x48,0x89,0x44,0x24,0x58,0x4c,0x8b}};
}
