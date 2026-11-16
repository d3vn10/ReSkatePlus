#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::gameplay_settings {
inline constexpr std::uintptr_t fast_travel_getter = 0x87b640;
inline constexpr std::uintptr_t activity_settings_handle = 0x725cc18;
inline constexpr std::uintptr_t activity_settings_vtable = 0x6146340;
inline constexpr std::uintptr_t advance_settings_handle = 0x725da00;
inline constexpr std::uintptr_t advance_settings_vtable = 0x61464a0;
inline constexpr std::uintptr_t player_progression_settings_handle = 0x7243cb0;
inline constexpr std::uintptr_t player_progression_settings_vtable = 0x612ebb0;
inline constexpr std::uintptr_t ui_settings_handle = 0x7257fe8;
inline constexpr std::uintptr_t ui_settings_vtable = 0x6141688;
inline constexpr std::uintptr_t progression_settings_handle = 0x725d5f0;
inline constexpr std::uintptr_t progression_settings_vtable = 0x61463e8;
inline constexpr std::uintptr_t force_all_activities_enabled_member = 0x82ff060;
inline constexpr std::uintptr_t use_dev_enabled_activities_member = 0x82ff090;
inline constexpr std::uintptr_t enable_quest_system_member = 0x82ff120;
inline constexpr std::uintptr_t enable_action_system_member = 0x82ff240;
inline constexpr std::uintptr_t enable_narrative_member = 0x82ff318;
inline constexpr std::uintptr_t enable_activity_spawned_waypoints_member = 0x82ff4e0;
inline constexpr std::uintptr_t enable_quest_system_debug_member = 0x82ff138;
inline constexpr std::uintptr_t force_use_mock_quest_member = 0x82ff1f8;
inline constexpr std::uintptr_t show_local_quests_member = 0x82ff288;
inline constexpr std::uintptr_t auto_claim_quests_member = 0x82ff2d0;
inline constexpr std::uintptr_t disable_quest_order_filtering_member = 0x82ff330;
inline constexpr std::uintptr_t dev_workflows_enabled_member = 0x82ff498;
inline constexpr std::uintptr_t fast_travel_points_enabled_member = 0x82fea00;
inline constexpr std::uintptr_t board_wear_enabled_member = 0x82fe9b8;
inline constexpr std::uintptr_t player_vs_player_collision_enabled_member = 0x82fe9d0;
inline constexpr std::uintptr_t enable_player_progression_member = 0x82ed7e8;
inline constexpr std::uintptr_t enable_graphs_member = 0x82ed800;
inline constexpr std::uintptr_t enable_dev_only_menu_member = 0x82fa6f8;
inline constexpr std::uintptr_t enable_content_browser_ui_member = 0x82fa710;
inline constexpr std::uintptr_t enable_legacy_progression_menu_member = 0x82fa728;
inline constexpr std::uintptr_t enable_new_map_member = 0x82fa8f0;
inline constexpr std::uintptr_t enable_old_buildkit_object_browser_member = 0x82faa10;
inline constexpr std::uintptr_t enable_neighborhood_rank_member = 0x82fe0d0;
inline constexpr std::uintptr_t enable_competitive_rank_member = 0x82fe0e8;

}
