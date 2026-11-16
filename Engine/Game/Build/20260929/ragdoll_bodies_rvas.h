#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::ragdoll_bodies {
inline constexpr std::uintptr_t system_generations_offset = 0x680; // system: uint32 per slot
inline constexpr std::uintptr_t board_handles_offset = 0x718;
inline constexpr std::uintptr_t rig_handles_offset = 0x6f8; // core

}
