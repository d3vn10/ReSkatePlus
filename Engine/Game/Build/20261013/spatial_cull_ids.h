// Reviewed native contract for supported_build::game_sha256 (September 29 2026).
// A spatial transaction queue (SpatialQueryManager's, and one in each world's client and render
// spatial managers) hands every object it tracks a cull ID from a free list. The list is made
// once, when the queue is, at SpatialQueryManagerSettings.InitialCullIdListSize entries (the
// game's config: 40,000; 80,000 in GameView.cfg), and never grows: an insert with the list empty
// reads entry 0x00000000 of it and crashes (0x489cfe9, loading a large custom map).
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include "spatial_cull_ids_rvas.h"

namespace dingosdk::game::build::v20261013::spatial_cull_ids {
// make_list(list, allocator): looks the settings up, sizes the list's two arrays (u32 next IDs,
// u64 per-ID entries) to InitialCullIdListSize and chains them into the free list. Its only
// callers are the two queue constructors (0x489b518, 0x489ba10).
inline constexpr std::array<std::uint8_t, 15> make_list_entry{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// lea rdx, [settings type 0x77ae5e0] ... mov rcx, [settings manager 0x7482ce0];
// call settings_lookup; test rax, rax; je; mov esi, [rax + 0x00]; jmp; mov esi, 1.
inline constexpr std::array<std::uint8_t, 7> settings_type_load{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::array<std::uint8_t, 27> size_read{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::size_t initial_cull_id_list_size = 0x00;
} // namespace dingosdk::game::build::v20261013::spatial_cull_ids
