#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::ui_pointer {
inline constexpr std::uintptr_t mouse_force_count = 0x14;
inline constexpr std::uintptr_t mouse_forced = 0x18;
inline constexpr std::uintptr_t mouse_cursor_requested = 0x19;
inline constexpr std::uintptr_t mouse_cursor_applied = 0xe9;

}
