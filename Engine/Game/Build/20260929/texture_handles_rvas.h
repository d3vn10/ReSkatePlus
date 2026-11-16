#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::texture_handles {
inline constexpr std::uintptr_t textures = 0x76e0010;
inline constexpr std::uintptr_t view_cache_insert_rva = 0x3e6dcf1;
inline constexpr std::uintptr_t view_cache_resume_rva = 0x3e6dcff;
inline constexpr std::uintptr_t view_cache_return_rva = 0x3e6df03;
inline constexpr std::uintptr_t view_cache = 0x76e9548;

}
