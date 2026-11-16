#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::native_player_ui {
inline constexpr std::uintptr_t map_tick = 0x685e40;
inline constexpr std::uintptr_t ui_copy = 0x667c90;
inline constexpr std::uintptr_t map_destroy = 0x66a730;
inline constexpr std::uintptr_t location_shutdown = 0x672230;
inline constexpr std::uintptr_t location_create = 0x676a30;
inline constexpr std::uintptr_t location_update = 0x678440;
inline constexpr std::uintptr_t location_release = 0x677350;
inline constexpr std::uintptr_t location_retain = 0x6763e0;
inline constexpr std::uintptr_t list_append = 0x67ca10;
inline constexpr std::uintptr_t map_project = 0x678dd0;
inline constexpr std::uintptr_t copy_call = 0x6860dc;
inline constexpr std::uintptr_t copy_return = 0x6860e1;
inline constexpr std::uintptr_t location_manager = 0x71f3970;
inline constexpr std::uintptr_t location_manager_vtable = 0x60cf5e8;
inline constexpr std::uintptr_t map_display_vtable = 0x60cf750;
inline constexpr std::uintptr_t map_display_interface_vtable = 0x60cf848;

}
