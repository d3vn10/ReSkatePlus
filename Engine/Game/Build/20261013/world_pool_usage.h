// Reviewed native contract for supported_build::game_sha256 (September 29 2026).
// Where the three world pools ReSkate enlarges (entity_pages.h, spatial_buckets.h)
// can be read for how full they are: for telling whether a level needs more than
// the game's own sizes.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include "world_pool_usage_rvas.h"

namespace dingosdk::game::build::v20261013::world_pool_usage {
// Entity snapshots: the page allocator, (owner, allocator, pages). It fills the
// snapshot's 128 page pointers at pages[0..127], each page zeroed when handed out,
// so the highest non-zero entry is the highest entity slot in use.
inline constexpr std::array<std::uint8_t, 10> entity_page_allocator_entry{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// The game's own entity slots (128 pages of 1,024).
inline constexpr std::size_t entity_slots_stock = 131'072;

// Render instances: the pool is [[global] + 0x000] + 0x0000 (as the one caller of
// its allocator loads it), and its next-index counter, taken with lock xadd, is
// the most it has ever handed out.
inline constexpr std::array<std::uint8_t, 27> render_instance_load{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::ptrdiff_t render_instance_manager_offset = 0x000;
inline constexpr std::ptrdiff_t render_instance_pool_offset = 0x0000;
inline constexpr std::array<std::uint8_t, 8> render_instance_counter{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // lock xadd [rdi+0x0000], esi
inline constexpr std::ptrdiff_t render_instance_counter_offset = 0x0000;
// The game's own render instances (512 pages of 512).
inline constexpr std::size_t render_instances_stock = 262'144;

// Spatial buckets: a query, (root, ...), four register arguments; the root's node
// count and each node's block count are at spatial_buckets.h's offsets.
inline constexpr std::array<std::uint8_t, 15> spatial_query_entry{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::ptrdiff_t spatial_nodes_offset = 0x000;
// The game's own blocks per bucket (its root bucket overflowed at 2,049).
inline constexpr std::size_t spatial_blocks_stock = 2'048;
} // namespace dingosdk::game::build::v20261013::world_pool_usage
