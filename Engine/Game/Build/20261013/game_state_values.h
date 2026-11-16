#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstddef>
#include <cstdint>

// The skater graph's named game-state values ("Animation/Dingo/EnumGS.PopHeight"...). The graph writes them through
// these setters, on job worker threads. Found 2026-10-10 with a hardware write watch (ReSkate research notes, "Tricks
// by command: the gesture graph's intents").
#include "game_state_values_rvas.h"

namespace dingosdk::game::build::v20261013::game_state_values {
// The setter for the graph's int and enum values: (setter, instance, value), where `setter` is the value's descriptor
// + descriptor_to_setter and `instance` the graph instance that owns the storage.

inline constexpr std::uintptr_t descriptor_to_setter = 0x00;
// The descriptor's name: a char*.
inline constexpr std::size_t descriptor_name = 0x00;
// The game's own grade of a pop, written once as the skater pops: 1 low, 2 medium, 3 high. Measured 2026-10-20,
// standing, quick flick: a crouch of 3-5 game ticks gave 1, 7-11 gave 2, 13 and up 3 (kickflip and nollie inward
// heelflip alike).
inline constexpr char pop_height_name[] = "Animation/Dingo/EnumGS.PopHeight";
} // namespace dingosdk::game::build::v20261013::game_state_values
