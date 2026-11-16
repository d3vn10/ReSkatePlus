#pragma once
#include "engine.h"
#include <array>
#include <cstdint>
#include "runtime_rvas.h"
namespace dingosdk::game::build::v20260929::runtime {
// Client game update (client vtable tick) hooked by the runtime controller.
inline constexpr std::array<unsigned char, 17> client_tick_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0,0,0};
// Main-menu expression hooked to catch session failures.
inline constexpr std::array<unsigned char, 25> menu_expression_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

// LevelDescription constructor and destructor.
// Returns whether a level asset loads through a local hosted server.
// Client vtable +0x00: submits a local level request.

// Code checked before the runtime controller starts.


// Pointer to the loaded-sublevel list object, its vtable and its listener vtable (+0x00).
// Type record whose uint16 variant offsets (+0x00, +0x00, +0x00, +0x00) the
// level description copy relies on.
// uint32 offset of the message bus in a game context.
}
