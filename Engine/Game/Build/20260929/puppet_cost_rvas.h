#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::puppet_cost {
inline constexpr std::uintptr_t update_info_size = 0x20;
inline constexpr std::uintptr_t update_info_dt = 8;
inline constexpr std::uintptr_t presim_handler_slot = 0x138;
inline constexpr std::uintptr_t postsim_handler_slot = 0x140;
inline constexpr std::uintptr_t navigator_vtable = 0x6078f30;
inline constexpr std::uintptr_t stim_vtable = 0x6438908;
inline constexpr std::uintptr_t collider_vtable = 0x6439560;
inline constexpr std::uintptr_t ecs_vtable = 0x61712b8;
inline constexpr std::uintptr_t ant_driven_vtable = 0x63aaee8;
inline constexpr std::uintptr_t ant_driven_hooks_vtable = 0x63aae90;
inline constexpr std::uintptr_t ant_driven_item = 0x50;
inline constexpr std::uintptr_t ant_driven_manager = 0x58;
inline constexpr std::uintptr_t ant_driven_animatable = 0x68;
inline constexpr std::uintptr_t manager_hooks = 0x40;
inline constexpr std::uintptr_t manager_hooks_storage = 0x98;
inline constexpr std::uintptr_t manager_kind = 0x90;

}
