// Reviewed native contract for supported_build::game_sha256 (September 29 2026).
// The size of the renderer's shader-visible CBV/SRV/UAV descriptor heaps: a persistent part
// (120,000) and, in each frame's heap, a part for that frame's descriptors: what is left of a
// total of 333,333 (a third of D3D12's 1,000,000), capped at 131,072.
#pragma once

#include <array>
#include <cstdint>
#include "descriptor_heap_size_rvas.h"

namespace dingosdk::game::build::v20260929::descriptor_heap_size {
// In the renderer's device setup (0x00000000), where the total each heap may have (333,333, from
// the device or from the platform's own description, PQXD; both paths join here, at the first
// instruction) is capped at 1,000,000 unless the device's resource binding is tier 3:
//   cmp dword ptr [rsi + 0x000], 3
//   je +0x00                           ; past the cap
//   mov edx, 1000000 / cmp ecx, edx / cmova ecx, edx   (left as they are)
inline constexpr std::array<std::uint8_t, 9> total_cap{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// In the renderer's device setup (0x00000000), the per-frame part: what the device allows past
// the persistent part, divided among the heaps, then capped by the setting at desc+8
// (0x00000000: no cap):
//   mov ecx, dword ptr [r14 + 8]       ; the cap
//   div r9d                            ; eax = room / heaps
//   mov edi, eax
//   mov eax, 0x00000000
//   cmp ecx, eax / je / cmp edi, ecx / cmovb ecx, edi / mov edi, ecx   (left as they are)
// No branch lands inside these four instructions.
inline constexpr std::array<std::uint8_t, 14> cap_load{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// Layout choice: one heap with a region per frame (sil = 0) or a heap per frame (sil = 1, PC).
inline constexpr std::array<std::uint8_t, 8> layout{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// Heap object constructor. Per-heap EASTL fixed vectors (4 inline): heaps +0x00, CPU starts +0x00,
// GPU starts +0x00. +0x00: frame regions per heap.
inline constexpr std::array<std::uint8_t, 15> heap_constructor{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::size_t heap_vectors[]{0x00, 0x00, 0x00};
inline constexpr std::size_t heap_regions = 0x00;
// Heap object's begin-frame (vtable slot 6): sets the frame's start (+0x000) and cursor (+0x000),
// then moves to the next region. +0x00: descriptors per frame region.
inline constexpr std::array<std::uint8_t, 14> begin_frame{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::size_t heap_per_frame = 0x00;
inline constexpr std::size_t heap_frame_start = 0x000;
inline constexpr std::size_t heap_cursor = 0x000;
} // namespace dingosdk::game::build::v20260929::descriptor_heap_size
