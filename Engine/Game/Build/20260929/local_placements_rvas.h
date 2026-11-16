#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::local_placements {
inline constexpr std::uintptr_t placement_manager_vtable = 0x60e0380;
inline constexpr std::uintptr_t player_component_offset = 0x6f7cf60;
inline constexpr std::uintptr_t client_ready_offset = 0x7215038;
inline constexpr std::uintptr_t teleport_manager = 0x71ee790;
inline constexpr std::uintptr_t object_ghost = 0x7ae620;
inline constexpr std::uintptr_t realm_context = 0x18b6e40;
inline constexpr std::uintptr_t objects_editable = 0x7a6a10;

inline constexpr std::uintptr_t thread_context = 0x18b6ae0;
inline constexpr std::uintptr_t object_ghost_decide = 0x7b33e0;
inline constexpr std::uintptr_t settings_lookup = 0x18d6ef0;
inline constexpr std::uintptr_t settings_manager = 0x7482ce0;
inline constexpr std::uintptr_t build_kit_settings = 0x723a510;
inline constexpr std::uintptr_t object_owned = 0x7ad210;
inline constexpr std::uintptr_t object_root = 0x6fdb00;
inline constexpr std::uintptr_t object_component = 0x46aa70;
}
