#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include "engine.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include "local_music_rvas.h"
namespace dingosdk::game::build::v20260929::local_music {
// Registered music assets
// Pointer to the music asset manager. Its CRITICAL_SECTION at +0x00 is taken
// through the game's IAT thunks (0x158fd80/0x1590860) by registration (0x43e260,
// keyed by "artist - title") and unregistration (0x449e70).
// MusicGraphAsset vtable.
// Music metadata (artist/title) vtable.
// Music TagRef vtable (enumeration + selection).
// Tag selection (enum value) vtable.

// Music UI
// Pointer to the music UI manager.
// Shared control block vtables of the playlist and song DTOs, and their expected slots.
// Playlist and song DTO constructors.
// Publish the playlist and song DTO lists into the UI manager.
// The music asset manager's own routines. Registers a song's asset under its "artist - title"
// (manager, asset); the asset of a song by that name (manager, char**), or 0; adds a song, by
// the 16 bytes before its asset, to the playlist of a number (manager, u32*, bytes).



// More of the manager's routines for the playlists the music screen makes: takes a song out of
// one (manager, u32*, bytes), empties one (manager, u32*), makes one (manager, u32* out).



// Asks a music queue for a song the player picked: (queue, asset). It looks the song up among
// those the queue can play and, when it is one, puts it on the queue's list of requests (which
// the selector takes from first).

// Begins to play a song's asset on a channel: (player, asset, channel, ...) -> the playing song.

// The music screen's song of a given id ("artist - title"), made when there is none:
// (manager, engine string*) -> its context in the model. Its field 1 is its cover's address.

// The music screen's own binding of the player's channel to a playlist: a return address in every
// binding traced that came from a playlist picked there, and in none the world made (2026-10-12).
// Compared against stack frames only.
// Builds a music shelf's data model entry (Extension/Music/local_music_shelf.cpp; hook target).

// Inserts a context into the UI manager's context map.
// Invokes the initialization completion delegate.
// Music UI initializer (hook target).

// CONFIRMED: queued selection setter (0x000000000) calls the change queue,
// then the apply-only setter (0x000000000). A nonzero fourth argument selects
// song favorites; zero selects playlist likes. The apply-only song path updates
// model field 2, the Favorites list and the playback playlist at manager +0x000.
// The playlist path updates field 7 and the Liked list at manager +0x000;
// disassembly at 0x000000000/0x000000000/0x000000000 confirms the split.


// Queue selector shared by UI Next and natural end-of-track selection. The queue
// supplies its own channel and registered playlist asset; do not hard-code IDs.

// The callback the queue selector hands the game's song scorer for a playlist asset with a
// selector: (u32 place, u32 *out, context) -> *out, a number of the song at that place of the
// asset's songs (song + 0x00), or 0 where the place holds none. The whole function (32 bytes):
// it does not look at how many songs the asset has.

// Native playlist playback-mode setter (PlaylistPlaybackMode, not MusicPlaylistType).

// Native channel-to-playlist binding entrypoint, used by the generated UI/script thunk.


// Playlist lookup crash guard. The shipped loop loads a song entry (rcx), masks it with
// `and rcx, -5` and tests a flag at [rcx+0x00]; an unresolved or NULL entry makes that
// dereference fault and crashes the game. The 13-byte patch replaces the leading 5-byte
// `mov eax, 0` with a 2-byte `xor eax, eax`, a 2-byte `jrcxz` that jumps past the whole
// dereference when rcx is zero, and a padding `nop`, so the untouched tail stays aligned.
inline constexpr std::array<unsigned char, 13> playlist_lookup_safety_contract{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::array<unsigned char, 13> playlist_lookup_patch_bytes{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// The jrcxz sits at +6 (after the 2-byte xor and 4-byte and) and ends at +8; its +0x00
// displacement lands on the loop increment (`inc r9d; add r8, 4`) instead of the fault.
inline constexpr std::size_t playlist_lookup_jump_next = 8;
inline constexpr std::size_t playlist_lookup_jump_displacement = 0x00;
inline constexpr std::uintptr_t playlist_lookup_jump_target =
    playlist_lookup_safety_rva + playlist_lookup_jump_next + playlist_lookup_jump_displacement;
}
