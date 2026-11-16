#pragma once
#include <array>
#include <cstdint>
#include "game_states_rvas.h"

namespace dingosdk::game::build::v20261013::game_states {
// Supported SHA-256 fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9.
// The skater's game states as the trainer reads and holds them (Extension/Trainer/trainer_gamestate.cpp).
// A game state is an asset object; its value is in one of the skater's state pages, which hang off
// its AntDrivenComponent (the same vtable as puppet_cost::ant_driven_vtable) in a block of pages
// known by its vtable.

// Hooked functions.
// The lookup of a state's value: (state object, context with the pages) -> where the value is, 0
// when the context has none. All of it is 0x00 bytes.
inline constexpr std::array<unsigned char, 14> state_lookup_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// The setter of a number state (a FloatAsset's virtual, `self` = object + 0x00, the value in the
// third argument): it finds the value with state_write_lookup and stores it.
inline constexpr std::array<unsigned char, 14> float_setter_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// The lookup of a state's value for writing: (context, object, create) -> where to write.
inline constexpr std::array<unsigned char, 15> state_write_lookup_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// The copy of one state into another (destination object and context, source context, source
// object or 0 for the destination's own): it finds both values and has the destination's type
// copy them. 0x11d9a7b is its call of state_write_lookup.
inline constexpr std::array<unsigned char, 15> state_copy_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// A rate controller's rate for this tick (float (controller*)): its time scale clamped to the min
// and max it copied from its asset. The per-tick update of each controller (0x10de730) asks it.
inline constexpr std::array<unsigned char, 13> rate_compute_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
}
