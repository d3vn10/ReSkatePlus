#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::native_player_ui {
inline constexpr std::uintptr_t map_tick = 0x6bd2c0;
inline constexpr std::uintptr_t ui_copy = 0x69d210;
inline constexpr std::uintptr_t map_destroy = 0x66a730;
inline constexpr std::uintptr_t location_shutdown = 0x672230;
inline constexpr std::uintptr_t location_create = 0x676a30;
inline constexpr std::uintptr_t location_update = 0x4400d0;
inline constexpr std::uintptr_t location_release = 0x43cf40;
inline constexpr std::uintptr_t location_retain = 0x439df0;
inline constexpr std::uintptr_t list_append = 0x6b5370;
inline constexpr std::uintptr_t map_project = 0x6b0120;
inline constexpr std::uintptr_t copy_call = 0x6860dc;
inline constexpr std::uintptr_t copy_return = 0x6860e1;
inline constexpr std::uintptr_t location_manager = 0x71f3970;
inline constexpr std::uintptr_t location_manager_vtable = 0x60cf5e8;
inline constexpr std::uintptr_t map_display_vtable = 0x60cf750;
inline constexpr std::uintptr_t map_display_interface_vtable = 0x60cf848;

}
