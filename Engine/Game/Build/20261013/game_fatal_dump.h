// Reviewed native contract for supported_build::game_sha256 (September 29 2026).
// The game's fatal handlers catch most crashes themselves (ReSkate's last-chance filter never
// sees them) and write their own minidump, of thread stacks only, in-process: three callers
// (0x18e3ea1 "Frostbite MiniDump", 0x2d28c9d, 0x2d2bfee) through one import thunk.
#pragma once
#include <array>
#include <cstdint>
#include "game_fatal_dump_rvas.h"

namespace dingosdk::game::build::v20260929::game_fatal_dump {
// MiniDumpWriteDump's import slot (api-ms-win-core-debug-minidump-l1-1-0.dll).
// The thunk every caller goes through: jmp qword ptr [import_slot].
inline constexpr std::array<std::uint8_t, 6> thunk_bytes{0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
} // namespace dingosdk::game::build::v20260929::game_fatal_dump
