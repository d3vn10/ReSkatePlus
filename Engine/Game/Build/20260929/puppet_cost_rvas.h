#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::puppet_cost {
inline constexpr std::uintptr_t navigator_vtable = 0x6078f30;
inline constexpr std::uintptr_t stim_vtable = 0x6438908;
inline constexpr std::uintptr_t collider_vtable = 0x6439560;
inline constexpr std::uintptr_t ecs_vtable = 0x61712b8;
inline constexpr std::uintptr_t ant_driven_vtable = 0x63aaee8;
inline constexpr std::uintptr_t ant_driven_hooks_vtable = 0x63aae90;

}
