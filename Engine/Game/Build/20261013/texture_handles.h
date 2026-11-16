#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include "texture_handles_rvas.h"

// The renderer's texture handles (Extension/World/texture_handle_guard.cpp).
//
// A render texture handle is {u16 id, u8 weak}: an id into the renderer's pool named "Texture"
// (the object at `textures` + 0x00, 0x00 bytes for each id). An entry holds the texture at + 0x00
// while it lives; when its generation ends and a live texture has the same name, the sweep
// (0x3e9833a, through 0x3e90110) gives the new entry that texture and points the old one at it
// by its id at + 0x00. resolve (0x3e897c0, and the same code inline at 0x3e8f262) reads + 0x00,
// then the entry + 0x00 names through redirect, then the pool's default for the id (0x3e89740):
// it never checks what redirect gives back. redirect answers null when + 0x00 is 0, and a
// material parameter that still holds the id of a texture whose last reference went (a
// ParamDbStateStream built for the render jobs keeps the bare id, 0x3ed5ac0) finds exactly
// that: an entry with no texture and no redirect. Both callers then read through null
// (Skate.exe+0x3e897fb, a third of 2.1.1's crash reports, every one with TheVlixOne-Nike_Pack
// 2.3.x installed; 2026-10-21).
//
// The "default for the id" (0x3e89740) is the entry itself, so resolve then answers the entry's
// own texture at + 0, which for a dead entry is null too: with an empty holder from redirect,
// resolve gives null instead of crashing. Its caller for a material's texture parameter (0x3f86270,
// slot 17 of the table at 0x64ff6e8) passes that through 0x3e61260 to the texture view cache
// (0x3e6dbb0, keyed by the texture at DAT 0x76e9548 + 0x000). On a miss the cache builds the view
// state without the texture (0x469a000 checks it for null and makes the same empty view the game
// makes for an empty slot), then, under its write lock from 0x3e6dcf1, asks the texture for its key
// (texture vtbl + 0x00) to keep the state: through null (Skate.exe+0x3e6ddc1, every report with the
// guard's line just before it and TheVlixOne-Nike_Pack installed; 2026-10-24). For a null texture
// the cache insert is skipped (`view_cache_insert`): no lock is held there, rsi is saved on every
// path to it, and no branch lands inside its two instructions.
//
// What remained in 2.1.3 (Sentry, 2026-11-02: Skate.exe+0x3e897fb, +0x3e89803, +0x3e89809 and,
// through the view cache, +0x469a020 / +0x469a035; 1,900 reports from 1,650 installs, nearly all with
// TheVlixOne-Nike_Pack): resolve finds a holder at + 0x00, or through redirect, that is not one.
// The faults read 0x0000, 0x0000, 0x00000000 (1.0f), 0x00000000 and non-canonical addresses: an id
// whose entry is free (its memory reused by the pool) or never held a texture, as when a parameter
// that is not a texture is read as an id. Every texture's vtable lies in the image, and resolve calls
// its slots + 0x00 and + 0x00 before answering, so the guard checks the holder, the texture and
// those two slots before resolve reads through them, and answers no texture otherwise. Every caller
// of resolve takes null (0x3f86270 and 0x469a000 as above; the others store it or check it).
namespace dingosdk::game::build::v20261013::texture_handles {
// The pool within the object `textures` points at.
inline constexpr std::uint32_t pool_offset = 0x00;
// resolve(handle) -> the texture a {u16 id, u8 weak} handle names, retained; the entry's own
// texture (null for a dead entry) when its holder is empty.

// lookup(pool, id) -> the entry's holder slot (entry + 0x00); makes the id's bucket if it has none.

// The texture's vtable slots resolve calls on what it found, one after the other.



// In the view cache (0x3e6dbb0), the texture in r15, the built state in r14:
//   mov rbx, [rip + 0x00000000]   ; DAT 0x76e9548, the cache
//   add rbx, 0x000                ; its write lock
// the two complete instructions moved into the stub, which goes on at `view_cache_resume`, or for a
// null texture to `view_cache_return` (mov rsi, [rsp + 0x00]; mov rax, r14; the cookie check; ret).
inline constexpr std::array<unsigned char, 14> view_cache_insert{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
inline constexpr std::uint32_t view_cache_lock_offset = 0x000;
}
