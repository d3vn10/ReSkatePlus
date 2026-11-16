#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs. Keep these separate from build-independent layouts/contracts.
namespace dingosdk::game::build::v20260929::engine {
inline constexpr std::uintptr_t ui_control_destroy = 0x214930;
inline constexpr std::uintptr_t audio_manager_update = 0x43c2f0;
inline constexpr std::uintptr_t check_entitlements = 0x774340;
inline constexpr std::uintptr_t construct_error = 0x83de30;
inline constexpr std::uintptr_t animation_count_getter = 0x11e5040;
inline constexpr std::uintptr_t native_text_release = 0x1586050;
inline constexpr std::uintptr_t string_assign = 0x1587620;
inline constexpr std::uintptr_t current_context = 0x18b6ae0;
inline constexpr std::uintptr_t settings_lookup = 0x18d6ef0;
inline constexpr std::uintptr_t model_find = 0x19193b0;
inline constexpr std::uintptr_t query_context_constructor = 0x276f3e0;
inline constexpr std::uintptr_t board_render_pose = 0x2e48610;
inline constexpr std::uintptr_t assign_future = 0x3dc5bd0;
inline constexpr std::uintptr_t array_allocate = 0x3dc7540;
inline constexpr std::uintptr_t delegate_copy = 0x3dd5540;
inline constexpr std::uintptr_t delegate_destroy = 0x3dd5780;
inline constexpr std::uintptr_t set_customization_flag = 0x4654e10;
inline constexpr std::uintptr_t post_event_delayed_frames = 0x4664e80;
inline constexpr std::uintptr_t post_event_delayed_seconds = 0x4665040;
inline constexpr std::uintptr_t complete_promise = 0x58b1cf0;
inline constexpr std::uintptr_t delegate_invoke = 0x58b1d60;
inline constexpr std::uintptr_t card_info_construct = 0x59dde00;
inline constexpr std::uintptr_t controller_vtable = 0x606dc60;
inline constexpr std::uintptr_t allocator_adapter_vtable = 0x606df70;
inline constexpr std::uintptr_t source_manager_vtable = 0x6079940;
inline constexpr std::uintptr_t skater_audio_component_vtable = 0x607a500;
inline constexpr std::uintptr_t audio_manager_vtable = 0x607b520;
inline constexpr std::uintptr_t client_vtable = 0x60842c0;
inline constexpr std::uintptr_t client_skater_source_vtable = 0x6084fa8;
inline constexpr std::uintptr_t skater_entity_vtable = 0x6087908;
inline constexpr std::uintptr_t cosmetics_manager_vtable = 0x6089cc8;
inline constexpr std::uintptr_t server_vtable = 0x60e0890;
inline constexpr std::uintptr_t game_settings_vtable = 0x6145be0;
inline constexpr std::uintptr_t skater_component_vtable = 0x6170410;
inline constexpr std::uintptr_t board_component_vtable = 0x6170e38;
inline constexpr std::uintptr_t board_entity_vtable = 0x6171808;
inline constexpr std::uintptr_t board_data_vtable = 0x6186fa0;
inline constexpr std::uintptr_t blueprint_vtable = 0x6187008;
inline constexpr std::uintptr_t skater_appearance_vtable = 0x61c9ec0;
inline constexpr std::uintptr_t camera_vtable = 0x6255870;
inline constexpr std::uintptr_t board_holder_vtable = 0x63db690;
inline constexpr std::uintptr_t local_player_vtable = 0x63e2d30;
inline constexpr std::uintptr_t local_player_manager_vtable = 0x63e30a0;
inline constexpr std::uintptr_t server_player_vtable = 0x63e4108;
inline constexpr std::uintptr_t server_player_manager_vtable = 0x63e41d0;
inline constexpr std::uintptr_t cosmetic_slot_type = 0x72290b8;
inline constexpr std::uintptr_t persona_id_type = 0x723bcf8;
inline constexpr std::uintptr_t player_info_type = 0x72591d8;
inline constexpr std::uintptr_t board_blueprint_type = 0x7288f80;
inline constexpr std::uintptr_t reference_type = 0x7493030;
inline constexpr std::uintptr_t bool_type = 0x765d8e8;
inline constexpr std::uintptr_t uint32_type = 0x765da28;
inline constexpr std::uintptr_t int32_type = 0x765da68;
inline constexpr std::uintptr_t uint64_type = 0x765daa8;
inline constexpr std::uintptr_t int64_type = 0x765dae8;
inline constexpr std::uintptr_t float_type = 0x765db28;
inline constexpr std::uintptr_t string_type = 0x765dcc0;
inline constexpr std::uintptr_t context_partition_offset = 0x6fc2430;
inline constexpr std::uintptr_t context_type_offset = 0x6fc24b8;
inline constexpr std::uintptr_t context_client_connection_offset = 0x715d5e8;
inline constexpr std::uintptr_t context_player_manager_offset = 0x7161720;
inline constexpr std::uintptr_t backend_services = 0x71e17e0;
inline constexpr std::uintptr_t audio_manager = 0x71e3978;
inline constexpr std::uintptr_t appearance_manager = 0x71eaba0;
inline constexpr std::uintptr_t source_managers = 0x71ec560;
inline constexpr std::uintptr_t loadout_manager = 0x71ec8c8;
inline constexpr std::uintptr_t cosmetics_manager = 0x71ee610;
inline constexpr std::uintptr_t client_game_manager = 0x71ef898;
inline constexpr std::uintptr_t location_owner = 0x71f4288;
inline constexpr std::uintptr_t ui_manager = 0x71f5ef0;
inline constexpr std::uintptr_t game_server = 0x71f8ad0;
inline constexpr std::uintptr_t game_settings_handle = 0x725b018;
inline constexpr std::uintptr_t default_arena = 0x72f6e78;
inline constexpr std::uintptr_t empty_cstring = 0x72fe704;
inline constexpr std::uintptr_t settings_manager = 0x7482ce0;
inline constexpr std::uintptr_t ui_allocator = 0x74889e8;
inline constexpr std::uintptr_t entity_creation_ready = 0x75c26f0;
inline constexpr std::uintptr_t player_parent_offset = 0x75c44d8;
inline constexpr std::uintptr_t domain_owners = 0x7726d10;
inline constexpr std::uintptr_t tls_index = 0x78d40e0;

}
