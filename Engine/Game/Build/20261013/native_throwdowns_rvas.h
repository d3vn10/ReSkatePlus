#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::native_throwdowns {
inline constexpr std::uintptr_t player_in_activity = 0x834520;
inline constexpr std::uintptr_t mode_available = 0x807130;
inline constexpr std::uintptr_t player_info = 0x793080;
inline constexpr std::uintptr_t service_player_info = 0x6d9780;
inline constexpr std::uintptr_t send_event = 0x733250;
inline constexpr std::uintptr_t parameters = 0x852f60;
inline constexpr std::uintptr_t others_throwdowns_allowed = 0x7aaec0;
inline constexpr std::uintptr_t container_lookup = 0x873ed0;
inline constexpr std::uintptr_t string_assign_chars = 0x15889b0;
inline constexpr std::uintptr_t array_release = 0x7b7b50;
inline constexpr std::uintptr_t debug_variable_row = 0x7159028;
inline constexpr std::uintptr_t debug_variable_stub = 0x2d70d60;
inline constexpr std::uintptr_t player_identity_offset = 0x6f7cfb0;
inline constexpr std::uintptr_t player_id_virtual = 0x7ff930;
inline constexpr std::uintptr_t realm_context = 0x18b6e40;
inline constexpr std::uintptr_t client_player_vars_offset = 0x6f7bae0;
inline constexpr std::uintptr_t local_player_base_offset = 0x75c3588;
inline constexpr std::uintptr_t entity_world_manager = 0x74cd760;
inline constexpr std::uintptr_t map_object_store_vtable = 0x65d0e48;
inline constexpr std::uintptr_t participants_changed = 0x810e30;
inline constexpr std::uintptr_t player_name = 0x820fe0;
inline constexpr std::uintptr_t find_player_by_id = 0x86c9d0;
inline constexpr std::uintptr_t start_challenge = 0x5951470;
inline constexpr std::uintptr_t array_append_u32 = 0x6b5280;
inline constexpr std::uintptr_t array_allocate = 0x3ddf650; // prefix: array_allocate_prefix above
inline constexpr std::uintptr_t challenge_start_request = 0x7fe8a0;
inline constexpr std::uintptr_t string_assign = 0x1587620;
inline constexpr std::uintptr_t type_construct = 0x3dd73a0;
inline constexpr std::uintptr_t type_destroy = 0x3def610;
inline constexpr std::uintptr_t event_allocator = 0x72f6e78;
inline constexpr std::uintptr_t player_id_by_name = 0x854aa0;
inline constexpr std::uintptr_t leaderboard_score = 0x8109f0;
inline constexpr std::uintptr_t parse_int = 0x864dc0;
inline constexpr std::uintptr_t player_id = 0x86cf90;
inline constexpr std::uintptr_t leaderboard_event_handle = 0x801860;
inline constexpr std::uintptr_t leaderboard_write = 0x8194a0;
inline constexpr std::uintptr_t order_participants = 0x813660;
inline constexpr std::uintptr_t set_participants = 0x81b410;

inline constexpr std::uintptr_t empty_string = 0x72fe704;
inline constexpr std::uintptr_t string_destroy = 0x1586050;
inline constexpr std::uintptr_t find_player_by_name = 0x834d90;
inline constexpr std::uintptr_t player_persona_id = 0x822170;
inline constexpr std::uintptr_t bool_and = 0x16e7130;
inline constexpr std::uintptr_t bool_or = 0x16eb6e0;
inline constexpr std::uintptr_t bool_not = 0x16eb600;
inline constexpr std::uintptr_t component_lookup = 0x7215d0;
inline constexpr std::uintptr_t leaderboard_manager_type = 0x7225350;
}
