#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs. Keep these separate from build-independent layouts/contracts.
namespace dingosdk::game::build::v20261013::ai_skaters {
inline constexpr std::uintptr_t create_bus = 0x2b7eb40;
inline constexpr std::uintptr_t acquire_bus = 0x2b91b10;
inline constexpr std::uintptr_t set_property_binding = 0x544020;
inline constexpr std::uintptr_t binding_reader = 0x53eba0;
inline constexpr std::uintptr_t binding_read_site = 0x53ebbf;
inline constexpr std::uintptr_t physics_source_setter = 0xf9cf40;
inline constexpr std::uintptr_t asset_domain_lookup = 0x41b5780;
inline constexpr std::uintptr_t brain_tree_vtable = 0x6252b20;
inline constexpr std::uintptr_t brain_tree_update = 0x16f5560;
inline constexpr std::uintptr_t brain_tree_holder_vtable = 0x6252d60;
inline constexpr std::uintptr_t brain_tree_factory = 0x16f23a0;
inline constexpr std::uintptr_t skater_ai_vtable = 0x6170180;
inline constexpr std::uintptr_t skater_ai_update = 0xf97630;
inline constexpr std::uintptr_t skater_ai_factory = 0xf881d0;
inline constexpr std::uintptr_t ai_controller_create = 0x10373a0;
inline constexpr std::uintptr_t ai_controller_run = 0x10461e0;

}
