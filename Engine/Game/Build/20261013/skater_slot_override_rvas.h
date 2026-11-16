#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::skater_slot_override {
inline constexpr std::uintptr_t settings_constructor = 0xc834c0;
inline constexpr std::uintptr_t manager_constructor = 0x52d920;
inline constexpr std::uintptr_t native_resize = 0x537850;
inline constexpr std::uintptr_t native_selected = 0x539120;
inline constexpr std::uintptr_t native_select = 0x5409f0;
inline constexpr std::uintptr_t slot_list_publisher = 0x4f91a0;
inline constexpr std::uintptr_t loadout_settings_handle = 0x72597e8;
inline constexpr std::uintptr_t loadout_settings_vtable = 0x6142a40;
inline constexpr std::uintptr_t min_loadout_count_field = 0x82fd3c8;
inline constexpr std::uintptr_t max_loadout_count_field = 0x82fd3e0;
inline constexpr std::uintptr_t force_own_max_slots_field = 0x82fd4a0;
inline constexpr std::uintptr_t show_prototype_cas_slots_field = 0x82ffd00;
inline constexpr std::uintptr_t show_new_character_preset_selector_field = 0x82fffb8;

}
