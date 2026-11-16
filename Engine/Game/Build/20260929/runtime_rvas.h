#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs. Keep these separate from build-independent layouts/contracts.
namespace dingosdk::game::build::v20260929::runtime {
inline constexpr std::uintptr_t client_tick = 0x2e77190;
inline constexpr std::uintptr_t menu_expression = 0x2d39900;
inline constexpr std::uintptr_t level_description_init = 0x5c6dcd0;
inline constexpr std::uintptr_t level_description_destroy = 0x50ddf0;
inline constexpr std::uintptr_t hosted_level_selector = 0x2f60fe0;
inline constexpr std::uintptr_t local_level_request = 0x2e72bb0;
inline constexpr std::uintptr_t sublevel_list = 0x75c26f8;
inline constexpr std::uintptr_t sublevel_list_vtable = 0x63e1e70;
inline constexpr std::uintptr_t sublevel_listener_vtable = 0x63e1f40;
inline constexpr std::uintptr_t level_description_layout = 0x83a2180;
inline constexpr std::uintptr_t context_message_bus_offset = 0x7482e18;

}
