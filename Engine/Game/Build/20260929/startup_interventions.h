#pragma once
#include <array>
#include <cstdint>
#include "startup_interventions_rvas.h"

namespace dingosdk::game::build::v20260929::startup_interventions {
// Build 25414733 clears the saved native frame through the VM's byte store,
// not the 20260908 REP STOSB handler. Capture before its first zero byte.
inline constexpr std::array<std::uint8_t, 3> frame_instruction{0x40, 0x88, 0x32}; // MOV [RDX], SIL
inline constexpr std::array<std::uint8_t, 8> frame_fingerprint{
    0x40, 0x88, 0x32, 0x5e, 0x66, 0x8b, 0x41, 0x08};
inline constexpr std::uint64_t frame_count = 0xdaf400c0;
inline constexpr std::array<std::uint8_t, 6> exit_instruction{
    0xff, 0x25, 0x7f, 0x43, 0x5e, 0xfd};
inline constexpr std::uintptr_t frame_size = 0xc0;
inline constexpr std::uintptr_t frame_stack_offset = 0xb70;
inline constexpr std::uintptr_t frame_vm_offset = 0xb40;
inline constexpr std::uintptr_t frame_saved_registers_offset = 0x1b0;
inline constexpr std::uintptr_t frame_cursor_offset = 0xd8;
inline constexpr std::uintptr_t exit_stack_offset = 0x128;

}
