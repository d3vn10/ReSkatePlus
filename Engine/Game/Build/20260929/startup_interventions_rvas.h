#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::startup_interventions {
inline constexpr std::uintptr_t frame = 0x08ae2785;
inline constexpr std::uintptr_t frame_data = 0x08b7bf3e; // R11: clear bytecode
inline constexpr std::uintptr_t frame_cursor = 0x08b7bf72; // [RCX + 0xd8]
inline constexpr std::uintptr_t frame_size = 0xc0;
inline constexpr std::uintptr_t frame_stack_offset = 0xb70;
inline constexpr std::uintptr_t frame_vm_offset = 0xb40;
inline constexpr std::uintptr_t frame_saved_registers_offset = 0x1b0;
inline constexpr std::uintptr_t frame_cursor_offset = 0xd8;
inline constexpr std::uintptr_t exit_stack_offset = 0x128;
inline constexpr std::uintptr_t exit_thunk = 0x08a06ff3;
inline constexpr std::uintptr_t exit_return = 0x087fe7fb;

}
