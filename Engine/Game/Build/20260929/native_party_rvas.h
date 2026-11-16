#pragma once
#include <cstdint>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::native_party {
inline constexpr std::uintptr_t model_record = 0x1915650;
inline constexpr std::uintptr_t model_type = 0x1917d70;
inline constexpr std::uintptr_t model_singleton = 0x19192b0;
inline constexpr std::uintptr_t get_info = 0x6d9750;
inline constexpr std::uintptr_t is_member = 0x778fb0;
inline constexpr std::uintptr_t member_ids = 0x776b10;
inline constexpr std::uintptr_t members_with_leader = 0x776d60;
inline constexpr std::uintptr_t weak_player = 0x848e90;
inline constexpr std::uintptr_t weak_entity = 0x834c20;
inline constexpr std::uintptr_t world_transform = 0x832930;
inline constexpr std::uintptr_t player_id = 0x82ff60;
inline constexpr std::uintptr_t can_spectate = 0x837380;
inline constexpr std::uintptr_t can_teleport = 0x837440;
inline constexpr std::uintptr_t spectate = 0x766fc0;
inline constexpr std::uintptr_t spectate_active = 0x7572a0;
inline constexpr std::uintptr_t spectate_clear = 0x55f090;
inline constexpr std::uintptr_t spectate_name = 0x75ae30;
inline constexpr std::uintptr_t teleport = 0x8615b0;
inline constexpr std::uintptr_t array_append = 0x6b8940;
inline constexpr std::uintptr_t id_construct = 0x58b0120;
inline constexpr std::uintptr_t empty_array = 0x3dcbb00;
inline constexpr std::uintptr_t array_free = 0x3dcc3a0;
inline constexpr std::uintptr_t poi_construct = 0x59b1e00;
inline constexpr std::uintptr_t poi_destroy = 0x59b8f80;
inline constexpr std::uintptr_t map_register = 0x6a1d60;
inline constexpr std::uintptr_t map_unregister = 0x6b98c0;
inline constexpr std::uintptr_t roster_changed = 0x6ece40;
inline constexpr std::uintptr_t player_list_type = 0x7493000;
inline constexpr std::uintptr_t uint64_array_type = 0x765dac8;
inline constexpr std::uintptr_t uint8_type = 0x765d928;
inline constexpr std::uintptr_t vec2_type = 0x765d278;
inline constexpr std::uintptr_t matrix_type = 0x765d590;
inline constexpr std::uintptr_t sorted_friends_type = 0x7250940;
inline constexpr std::uintptr_t map_poi_type = 0x724d3d0;
inline constexpr std::uintptr_t map_data_type = 0x724d4e8;
inline constexpr std::uintptr_t map_poi_kind_type = 0x724cf98;
inline constexpr std::uintptr_t friends_service = 0x71f4270;
inline constexpr std::uintptr_t friends_service_vtable = 0x60d2068;
inline constexpr std::uintptr_t map_manager = 0x71f4258;
inline constexpr std::uintptr_t map_manager_vtable = 0x60d21a8;
inline constexpr std::uintptr_t request_invite = 0x84be30;      // (Handle*, const u64* eaid, Delegate*)
inline constexpr std::uintptr_t request_join_group = 0x7797a0;  // (Handle*, u64 group, i32 reason (1 direct, 2 invite), Delegate*)
inline constexpr std::uintptr_t request_join_player = 0x77a0e0; // (Handle*, const u64* eaid, Delegate*)
inline constexpr std::uintptr_t request_decline = 0x84afc0;     // (Handle*, const String* invite id, Delegate*)
inline constexpr std::uintptr_t request_leave = 0x77a420;       // (Handle*, Delegate*)
inline constexpr std::uintptr_t request_kick = 0x77a1f0;        // (Handle*, const u64* eaid, Delegate*)
inline constexpr std::uintptr_t request_promote = 0x77a5c0;     // (Handle*, const u64* eaid, Delegate*)
inline constexpr std::uintptr_t request_privacy = 0x839f10;     // (u32 setting (4 invite, 5 join), u32 value)
inline constexpr std::uintptr_t local_is_leader = 0x779710;    // (bool* out)
inline constexpr std::uintptr_t in_group = 0x849d70;           // () -> bool
inline constexpr std::uintptr_t player_group_status = 0x775ee0; // (const u64* eaid, GameGroupStatus* out, bool* found)
inline constexpr std::uintptr_t voice_unavailable = 0x848e60;

}
