#pragma once
#include "engine.h"
#include <array>
#include <cstdint>
#include "runtime_rvas.h"
namespace dingosdk::game::build::v20260929::runtime {
// Client game update (client vtable tick) hooked by the runtime controller.
inline constexpr std::array<unsigned char, 17> client_tick_prefix{
    0x40,0x53,0x57,0x41,0x55,0x41,0x56,0x48,0x83,0xec,0x38,0x8b,0x81,0xc4,0,0,0};
// Main-menu expression hooked to catch session failures.
inline constexpr std::array<unsigned char, 25> menu_expression_prefix{
    0x40,0x53,0x55,0x56,0x57,0x48,0x83,0xec,0x38,0x80,0x79,0x28,0x00,
    0x49,0x8b,0xd9,0x49,0x8b,0x30,0x48,0x8b,0xf9,0x48,0x8b,0x2a};

// LevelDescription constructor and destructor.
// Returns whether a level asset loads through a local hosted server.
// Client vtable +0x70: submits a local level request.

// Code checked before the runtime controller starts.


// Pointer to the loaded-sublevel list object, its vtable and its listener vtable (+0x40).
// Type record whose uint16 variant offsets (+0x20, +0x50, +0x68, +0x80) the
// level description copy relies on.
// uint32 offset of the message bus in a game context.
}
