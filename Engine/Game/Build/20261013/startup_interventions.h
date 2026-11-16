#pragma once
#include <array>
#include <cstdint>
#include "startup_interventions_rvas.h"

namespace dingosdk::game::build::v20261013::startup_interventions {
// Build 25414733 clears the saved native frame through the VM's byte store,
// not the 20260908 REP STOSB handler. Capture before its first zero byte.
inline constexpr std::array<std::uint8_t, 3> frame_instruction{0x00, 0x00, 0x00}; // MOV [RDX], SIL
inline constexpr std::array<std::uint8_t, 8> frame_fingerprint{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::uint64_t frame_count = 0x00000000;
inline constexpr std::array<std::uint8_t, 6> exit_instruction{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr std::uintptr_t frame_size = 0x00;
inline constexpr std::uintptr_t frame_stack_offset = 0x000;
inline constexpr std::uintptr_t frame_vm_offset = 0x000;
inline constexpr std::uintptr_t frame_saved_registers_offset = 0x000;
inline constexpr std::uintptr_t frame_cursor_offset = 0x00;
inline constexpr std::uintptr_t exit_stack_offset = 0x000;

}
