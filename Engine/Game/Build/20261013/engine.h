#pragma once
#include <cstdint>
#include "engine_rvas.h"
// Engine layout constants shared by several features. Executable addresses live in
// engine_rvas.h; offsets and other non-RVA layout data stay here.
namespace dingosdk::game::build::v20260929::engine {

// Functions
// UI control destructor shared by the cosmetic and music list vtables.
// Audio manager vtable slot +0x00.
// Entitlement check request.
// Constructs an engine error value.
// Animation definition count getter (definition+0x000 -> +0x0).
// Releases an engine-owned text handle.
// Returns the calling thread's game context.
// Settings manager lookup by descriptor.
// UI data model lookup.
// Game query context constructor.
// Skateboard render pose publisher.
// Assigns a future from a promise state.
// Engine array allocation.
// Sets a flag on a customization/indicator property.
// Table functions graphs bind PostEventDelayedFrames (0x00000000) and
// PostEventDelayedSeconds (0x00000000) calls to.
// Completes a promise with a result or error.
// Player card info constructor.

// Vtables
// Connection controller.
// Client game context; state at +0x00.
// Current game server.
// DelMarGameSettings.
// Blueprint objects, including the board and UI widget blueprints.
// Skater appearance entity.

// Type information
// CString.

// Globals
// uint32 offset of the partition record in a game context.
// uint32 offset of the type/realm record in a game context.
// uint32 offset of the client connection in a game context.
// uint32 offset of the player manager in a game context.
// Pointer.
// Pointer.
// Pointer.
// Pointer array indexed by partition * 7 + kind.
// Pointer.
// Pointer.
// Pointer.
// Pointer.
// Pointer.
// The UI manager's UI model registry (the ModelManager every UI model is made in), a pointer at this
// offset in it.
inline constexpr std::uint32_t ui_model_registry = 0x000;
// Pointer.
// DelMarGameSettings handle.
// Pointer.
// The engine's empty CString.
// Pointer.
// Pointer.
// uint8.
// uint32 registration offset between player base classes.
// Pointer array indexed by domain.
// uint32 engine TLS slot.
}
