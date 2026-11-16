#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstddef>
#include <cstdint>

// The game's sound engine (EA::Audio::Core): the plugin that plays a streamed sound file
// ("SndPlayer1"), which is what the music player's songs come out of.
#include "audio_core_rvas.h"

namespace dingosdk::game::build::v20260929::audio_core {
// SndPlayer1's processing routine: (player, mixer) -> nonzero when it produced a block.

// SndPlayer1's command routine: (player, command, arguments). Command 4 asks it to play a
// file, whose name is the string at arguments + 0x00.

inline constexpr int command_play = 4;
inline constexpr std::size_t command_play_name = 0x00;

// The player.
inline constexpr std::size_t player_block_frames = 0x000; // u16: the most frames of one block
inline constexpr std::size_t player_voices = 0x000;       // u16: where its voices begin
inline constexpr std::size_t player_voice = 0x000;        // u8: the voice being played
inline constexpr std::size_t player_voice_count = 0x000;  // u8
inline constexpr std::size_t voice_size = 0x000;
inline constexpr std::size_t voice_rate = 0x00;           // float, Hz
inline constexpr std::size_t voice_frames = 0x00;         // u32: the length of the file
inline constexpr std::size_t voice_state = 0x00;          // u8: 0 and 5 are not playing
inline constexpr std::size_t voice_channels = 0x00;       // u8

// The mixer. A block is written to the buffer at mixer_write, and the two are then exchanged:
// after the routine, what it wrote is the buffer at mixer_written.
inline constexpr std::size_t mixer_written = 0x00000;
inline constexpr std::size_t mixer_write = 0x00000;
inline constexpr std::size_t mixer_frames = 0x00000;      // u32: frames of the block
inline constexpr std::size_t mixer_rate = 0x00000;        // float
inline constexpr std::size_t mixer_channels = 0x00000;    // u8
// A buffer: 32-bit float samples, a channel after another.
inline constexpr std::size_t buffer_data = 0x0;           // channel pointers, or the samples
inline constexpr std::size_t buffer_stride = 0x00;        // u16: samples a channel, when together
inline constexpr std::size_t buffer_together = 0x00;      // u8: nonzero when buffer_data is the samples
}
