#pragma once
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

}
