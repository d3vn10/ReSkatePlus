#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>

// The end of a streaming read that is scattered into place (Extension/World/streaming_copy_guard.cpp).
//
// finish(scheduler, request), on the I/O thread (0x42108e8 < 0x4213fde): when the request's flags
// (request + 0x00, bits 0 and 1) say its data is staged, it takes two staging buffers of 0x00000
// bytes, puts the data into the first (decompressing it, 0x41b9370, or copying the request's two
// pieces, request + 0 and + 8 of request + 0x00 and + 0x00 bytes), and then copies each of the
// request's places (bits 2 .. 23 of the flags count them) out of it: `count` times `size`
// bytes from the buffer at `offset` on, to `destination` on by `stride`. Nothing checks a place
// against the buffer. A mod's data whose header asks for more than its chunk has makes a place
// reach past the buffer's end, and the copy reads off the end of its allocation (VCRUNTIME140
// memcpy under 0x41bfacf, about 3,500 of 2.1.1's reports, every one copying 0x00000 or 0x00000
// bytes from the start of the buffer; 2026-10-21).
#include "streaming_reads_rvas.h"

namespace dingosdk::game::build::v20261013::streaming_reads {

inline constexpr std::ptrdiff_t places_offset = 0x00; // request + 0x00: the places
inline constexpr std::ptrdiff_t flags_offset = 0x00;  // request + 0x00: u32
inline constexpr std::uint32_t staged_flags = 3, place_count_shift = 2, place_count_mask = 0x3fffff;
inline constexpr std::uint32_t staging_size = 0x00000;
struct Place {
    std::uintptr_t destination;
    std::uint32_t size, stride, offset, count;
};
static_assert(sizeof(Place) == 24);
}
