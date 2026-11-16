#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::morph_memory_pool {
inline constexpr std::uintptr_t constructor_rva = 0x01b21580;
inline constexpr std::uintptr_t temporary_return = 0x01b0e724;
inline constexpr std::uintptr_t permanent_return = 0x01b0e6db;
inline constexpr std::uintptr_t temporary_pool = 0x074b3d80;
inline constexpr std::uintptr_t permanent_pool = 0x074b3d88;

}
