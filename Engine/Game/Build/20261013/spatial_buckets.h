#pragma once

#include "entity_pages.h"

namespace dingosdk::game::build::v20260929::spatial_buckets {
using entity_pages::Kind;
using entity_pages::Patch;
using entity_pages::patch;

// Each native block holds 64 bounds. The flat root bucket previously overflowed
// at 2049 blocks, aliasing its first block pointer and count with array writes.
// Keep native block ownership and IDs; enlarge every bucket array together.
inline constexpr std::size_t blocks_per_bucket = 16'384;
inline constexpr std::size_t entries_per_block = 64;
inline constexpr std::size_t nodes_per_root = 9;
struct BucketLayout {
    std::uint32_t block_ids[blocks_per_bucket];
    std::uint64_t block_pointers[blocks_per_bucket];
    std::uint32_t block_count;
    std::uint8_t occupancy[blocks_per_bucket];
    std::uint16_t changed_ids[blocks_per_bucket];
    std::uint16_t changed_count;
};
struct alignas(16) RootLayout {
    std::uint8_t prefix[0x000];
    BucketLayout nodes[nodes_per_root];
    std::uint32_t node_count;
};
// (Left at eight times the game's while the entity pages are at two: room to spare.)
static_assert(blocks_per_bucket * entries_per_block >= entity_pages::capacity);
static_assert(blocks_per_bucket < 65'536);
static_assert(offsetof(BucketLayout, block_pointers) == 0x00000);
static_assert(offsetof(BucketLayout, block_count) == 0x00000);
static_assert(offsetof(BucketLayout, occupancy) == 0x00000);
static_assert(offsetof(BucketLayout, changed_ids) == 0x00000);
static_assert(offsetof(BucketLayout, changed_count) == 0x00000);
static_assert(sizeof(BucketLayout) == 0x00000);
static_assert(offsetof(RootLayout, node_count) == 0x21c178);
static_assert(sizeof(RootLayout) == 0x21c180);

// Full instruction fingerprints: allocation/clear, queries, insert, move, remove
// and scalar root traversal. The final two records are the two uint64 lanes of
// the AVX-512 traversal's shared root-stride literal. Transaction queues, stack
// frames and independent render-packet arrays retain their original sizes.
inline constexpr Patch patches[]{
    patch(0x00000000, Kind::spatial_bucket_layout, "8b9178390400", "8b9178c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c808780000", "4869c808c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "8b8100600000", "8b8100000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "410fb6ac0c04600000", "410fb6ac0c04000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4e8bbce100200000", "4e8bbce100000100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "8b943978390400", "8b943978c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4c69f808780000", "4c69f808c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "458baf00600000", "458baf00000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "420fb6b43d04600000", "420fb6b43d04000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "498bbcef00200000", "498bbcef00000100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "8b943978390400", "8b943978c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4c69e808780000", "4c69e808c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "418b8500600000", "418b8500000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4f8bb4e500200000", "4f8bb4e500000100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "470fb6bc2c04600000", "470fb6bc2c04000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "8b9a78390400", "8b9a78c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4c69c008780000", "4c69c008c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "458bb000600000", "458bb000000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "420fb68c0204600000", "420fb68c0204000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "420fb6840004600000", "420fb6840004000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "420fb68c0204600000", "420fb68c0204000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c808780000", "4869c808c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "8b9100600000", "8b9100000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "488bb4c100200000", "488bb4c100000100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "0fb6840804600000", "0fb6840804000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869d080390400", "4869d080c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "8b943978390400", "8b943978c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4c69e808780000", "4c69e808c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "418b8500600000", "418b8500000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "420fb6bc2d04600000", "420fb6bc2d04000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "498bb4ed00200000", "498bb4ed00000100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "8b9178390400", "8b9178c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4c69e808780000", "4c69e808c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "418b8500600000", "418b8500000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "430fb6b42e04600000", "430fb6b42e04000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4b8bacf500200000", "4b8bacf500000100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "8b943178390400", "8b943178c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4c69e808780000", "4c69e808c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "418b8500600000", "418b8500000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "430fb6b42e04600000", "430fb6b42e04000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4b8bacf500200000", "4b8bacf500000100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "ba80390400", "ba80c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "41b880390400", "41b880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "c7807839040001000000", "c78078c1210001000000"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869de80390400", "4869de80c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "c784017839040001000000", "c7840178c1210001000000"),
    patch(0x00000000, Kind::spatial_bucket_layout, "418b8e30610000", "418b8e30010300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4280bc333461000040", "4280bc333401030040"),
    patch(0x00000000, Kind::spatial_bucket_layout, "49898e30210000", "49898e30010100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "41898e30610000", "41898e30010300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "420fb6b43334610000", "420fb6b43334010300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "41898e30610000", "41898e30010300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4989bcde30210000", "4989bcde30010100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "498bbcde30210000", "498bbcde30010100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4288b43334610000", "4288b43334010300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "410fb79634790000", "410fb79634c10300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "6641399c4e34690000", "6641399c4e34410300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "6641899c5634690000", "6641899c5634410300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "6641899634790000", "6641899634c10300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869f908780000", "4869f908c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "0fb68c3e04600000", "0fb68c3e04000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4c8b8cf700200000", "4c8b8cf700000100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "0fb79704780000", "0fb79704c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "6639b44704680000", "6639b44704400300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "6689b45704680000", "6689b45704400300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "66899704780000", "66899704c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c180390400", "4869c180c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4c69c908780000", "4c69c908c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "430fb68c0b04600000", "430fb68c0b04000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4b8b9cd900200000", "4b8b9cd900000100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "43fe8c0b04600000", "43fe8c0b04000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "410fb79104780000", "410fb79104c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "6645399c4904680000", "6645399c4904400300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "6645899c5104680000", "6645899c5104400300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "6641899104780000", "6641899104c00300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869f880390400", "4869f880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869d080390400", "4869d080c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869c880390400", "4869c880c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "448ba200600000", "448ba200000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "0fb6bc0604600000", "0fb6bc0604000300"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4c8bb4c600200000", "4c8bb4c600000100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "8ba978390400", "8ba978c12100"),
    patch(0x00000000, Kind::spatial_bucket_layout, "4869d008780000", "4869d008c00300"),
    patch(0x00000000, Kind::spatial_bucket_stride, "8039040000000000", "80c1210000000000"),
    patch(0x00000000, Kind::spatial_bucket_stride, "8039040000000000", "80c1210000000000"),
};
}

