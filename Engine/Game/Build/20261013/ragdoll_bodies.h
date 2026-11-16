#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstddef>
#include <cstdint>
#include "ragdoll_bodies_rvas.h"

namespace dingosdk::game::build::v20260929::ragdoll_bodies {
// Supported SHA-256 fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9.
// Real Bails (Extension/Skater/real_bails.h): the skater's ragdoll bodies in the physics world itself.
//
// The rig's 26 body records (skater_state.h: rig physics +0x00, 0x000 each) are the game's mirror of the world: in a
// bail it fills them from the world every step and drops what is written to them (2026-11-01: a spin or velocity
// written with its pending flag, before or after the skater's velocity update, the skeleton response or the animation
// publication, was gone the next step every time, the flag cleared). The world's own bodies are reached by handle,
// as the game's board sync (0x000000000, called from 0x000000000 with the board holder's handles at +0x000) does:
// 16 bytes, the physics system, the body's slot and the slot's generation. A handle is current while the system's
// generation array (+0x000) holds the same generation for the slot (0x000000000).
struct BodyHandle {
    std::uintptr_t system{};
    std::uint32_t slot{}, generation{};
};
static_assert(sizeof(BodyHandle) == 16);



// The board's handles (the board holder, core+0x000, +0x000: one per board body).
// The ragdoll's handles: in the skater's core from +0x000, one per ragdoll body, handle k for the rig's body k + 1
// (skater_body.h: head first, pelvis last). Found 2026-11-01 by the board's physics system pointer turning up there
// 23 times in a row, then confirmed in a bail: a spin set through handle 8 turned the right upper arm (body 9), 17 the
// left thigh, 21 the right thigh, 1 the neck. The entry after the 23rd is empty (slot 0x00000000).
inline constexpr std::size_t rig_handle_count = 23;
inline constexpr std::uintptr_t system_generations_offset = 0x000; // system: uint32 per slot
inline constexpr std::uintptr_t board_handles_offset = 0x000;
inline constexpr std::uintptr_t rig_handles_offset = 0x000; // core

}
