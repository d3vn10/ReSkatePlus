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

}
