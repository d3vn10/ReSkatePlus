#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::skater_entities {
inline constexpr std::uintptr_t blueprint_type = 0x7586ce8;
inline constexpr std::uintptr_t object_blueprint_type = 0x75043a8;
inline constexpr std::uintptr_t object_data_offset = 0x80;            // blueprint
inline constexpr std::uintptr_t object_data_vtable = 0x64062a0;
inline constexpr std::uintptr_t object_data_mesh_offset = 0xd8;       // object data -> mesh asset
inline constexpr std::uintptr_t mesh_asset_vtable = 0x64e6f30;
inline constexpr std::uintptr_t mesh_asset_loaded_offset = 0x70;      // mesh asset -> loaded mesh
inline constexpr std::uintptr_t loaded_mesh_vtable = 0x64f2ba8;
inline constexpr std::uintptr_t loaded_mesh_bounds_offset = 0x28;     // loaded mesh -> {min, max}
inline constexpr std::uintptr_t source_apply_appearance = 0x511d20;
inline constexpr std::uintptr_t descriptor_init = 0x2e90d90;
inline constexpr std::uintptr_t descriptor_destroy = 0x4ca880;
inline constexpr std::uintptr_t create_entity = 0x2e93070;
inline constexpr std::uintptr_t destroy_entity = 0x2b689f0;
inline constexpr std::uintptr_t release_reference = 0x2b808d0;
inline constexpr std::uintptr_t place_entity = 0x53ed70;
inline constexpr std::uintptr_t move_spatial_entity = 0x2f7ebc0;
inline constexpr std::uintptr_t initialize_placement = 0x2f7d670;
inline constexpr std::uintptr_t customization_component = 0x53f0e0;
inline constexpr std::uintptr_t physics_c7_setter = 0xf9cac0;
inline constexpr std::uintptr_t physics_cb_setter = 0xf9d020;
inline constexpr std::array<std::uintptr_t, 2> skater_component_base_vtables{0x61705e8, 0x6170650};

}
