#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs. Keep these separate from build-independent layouts/contracts.
namespace dingosdk::game::build::v20260929::native_skater {
inline constexpr std::uintptr_t animation_update = 0xf9ace0;
inline constexpr std::uintptr_t board_render_pose_interface_vtable = 0x63db790;
inline constexpr std::uintptr_t submit_render_pose = 0x4c7e7a0;
inline constexpr std::uintptr_t entity_helper = 0x2b897b0;
inline constexpr std::uintptr_t set_blueprint_transform = 0x2b7d3b0;
inline constexpr std::uintptr_t blueprint_transform_vtable = 0x6384e18;
inline constexpr std::uintptr_t place_board = 0xf98db0;
inline constexpr std::uintptr_t place_board_rig_call = 0xf98db0;
inline constexpr std::uintptr_t board_world_transform_event = 0xf97eb0;
inline constexpr std::uintptr_t board_placement_callback = 0xfa3140;
inline constexpr std::uintptr_t enable_board_resources = 0x2e62f20;
inline constexpr std::uintptr_t board_startup = 0xf97880;
inline constexpr std::uintptr_t board_physics_create = 0xf96e70;
inline constexpr std::uintptr_t initialize_creation_list = 0x2eaaa40;
inline constexpr std::uintptr_t free_creation_page = 0x2715360;
inline constexpr std::uintptr_t component_cache_callback = 0x53fe80;
inline constexpr std::uintptr_t board_from_component = 0x53f4e0;
inline constexpr std::uintptr_t board_animation_holder = 0xf95310;
inline constexpr std::uintptr_t pose_output_resolve = 0x12e2e50;
inline constexpr std::array<std::uintptr_t, 2> board_holder_resource_methods{0x2e4a410, 0x1f9630};

}
