#pragma once
#include <array>
#include <cstdint>
#include "trick_rewrite_rvas.h"

namespace dingosdk::game::build::v20261013::trick_rewrite {
// Supported SHA-256 fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9.
// Found by TheRealEJR for the Rock to Fakie mod (TheRealEJR/ReSkate-Rock-to-Fakie-mod v1.4.15 to
// v1.4.42, pull request 285), with a read-only recorder of the local skater's physics states, the
// trick recorder's packets and the localized labels the trick ticker asks for.

// void record(recorder, const packet*): the trick recorder. It copies the 104-byte packet into its
// history before it returns. Packet +0x00 is the trick id (at most 0x000), +0x00 nonzero when valid.
// In the same catalogue a Board Stall records id 9 (bs_board), a Lip Stall id 12 (bs_lip), and 187 is
// the game's own fakie_rock (the internal-name lookup at 0x4783030 resolves all three).
inline constexpr std::array<unsigned char, 32> record_trick_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::size_t trick_packet_size = 0x00;
inline constexpr std::uintptr_t trick_packet_id = 0x00, trick_packet_valid = 0x00;
inline constexpr std::uint32_t trick_id_max = 0x000;

// bool resolve(object, const char* key, char* buffer, uint32 capacity): a virtual method (found
// only in a vtable, no direct calls) that writes a key's localized text into the caller's buffer;
// AL is true on success. The trick ticker's labels come through it ("Board Stall", "Lip Stall"),
// with these three return addresses as frames 2, 3 and 4 of the call's unwound stack (frame 0 the
// hook, frame 1 its caller). No label but the ticker's was seen on that route.
inline constexpr std::array<unsigned char, 11> resolve_localized_key_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
}
