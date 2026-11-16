#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include "ui_pointer_rvas.h"

// The UI's per-frame pointer hit-test and the mouse cursor mode that says whether it matters
// (Extension/UI/ui_pointer_skip.cpp; analysis/sim-performance.md).
namespace dingosdk::game::build::v20260929::ui_pointer {
// Every client frame the input dispatch (0x31ff690) runs the mouse state update (0x31fdbf0),
// which clears the player's hit list and hands the pointer to the UI's pointer handler (vtable
// 0x65a0bb0: +0x00 the pointer stayed still, +0x00 it moved; both thunk into these). They walk
// every UI view and, for each active input-taking one, hit-test the pointer down its widget
// tree (view vtable +0x00 / +0x00 = 0x4323f50 ...), filling the hit list clicks and hover use.


// InputManMouse. enable_cursor_mode(mouse, bool enable, int style, const char* reason) is the
// locked vtable entry (+0x000 of the vtable around 0x641c4a0) the UI's cursor entities call;
// force_cursor(mouse, int delta) is a counted override that keeps the cursor on (posted to the
// window thread as message 0x0000). Either one hands over the mouse object.


// The InputManMouse base constructor (vtable 0x641c4a0; the Win32 mouse's 0x6421200 and
// 0x641c978 derive from it), run once at engine start: it hands over the mouse before any
// cursor change.

// InputManMouse fields: the force count (> 0 keeps the cursor on), a deferral byte (0x00 from
// the constructor; while non-zero a cursor change is held back), the cursor mode asked for, and
// the one applied (bytes; 0 = no cursor).
inline constexpr std::uintptr_t mouse_force_count = 0x00;
inline constexpr std::uintptr_t mouse_forced = 0x00;
inline constexpr std::uintptr_t mouse_cursor_requested = 0x00;
inline constexpr std::uintptr_t mouse_cursor_applied = 0x00;

}
