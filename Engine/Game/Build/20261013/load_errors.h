#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>

// The game's errors of a load made of several (Extension/World/load_error_guard.cpp).
#include "load_errors_rvas.h"

namespace dingosdk::game::build::v20261013::load_errors {
// describe(errors, text): a virtual of the error that holds one error per part of a load. It
// writes "[i]:" and each part's own description (its virtual at + 8), or "no error" for a null
// one, a line each. The parts are an array of pointers (errors + 0x00) and their count
// (errors + 0x00); the text is the game's fixed text writer, whose first field is its buffer
// with flags in the two low bits.
//
// The game calls it when a load fails, to write "load <address> failed with error: ..."
// (0x1aa75e0), and the array can hold parts that were never set: crash reports of 2026-10-11
// (one player, six times, 2.0.5 and 2.0.6) all end in the call through part 8 after eight that
// read "no error" - once a null vtable, else garbage that was read or run.

inline constexpr std::uintptr_t parts_offset = 0x00, count_offset = 0x00;
}
