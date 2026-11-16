#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::no_bail {
inline constexpr std::uintptr_t teleport_on_board_option = 0xc0;
inline constexpr std::uintptr_t reset_bail_causes_rva = 0x474cca0;
inline constexpr std::uintptr_t body_contact_output_offset = 0xfb8;
inline constexpr std::uintptr_t body_contact_context_offset = 0x13c4;
inline constexpr std::uintptr_t animation_request_offset = 0x13d4;
inline constexpr std::uintptr_t impact_request_offset = 0x13c4;
inline constexpr std::uintptr_t bail_core_vtable = 0x6176d30;
inline constexpr std::uintptr_t recovery_predicate = 0x47d1100;
inline constexpr std::uintptr_t recovery_animation_output = 0x38; // core+3b8 -> trick state
inline constexpr std::uintptr_t recovery_animation_flag = 0x9e;   // animation output, byte

}
