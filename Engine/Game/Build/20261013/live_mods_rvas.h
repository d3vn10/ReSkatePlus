#pragma once
#include <cstdint>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::live_mods {
inline constexpr std::uintptr_t register_superbundle = 0x41de0e0;
inline constexpr std::uintptr_t find_superbundle = 0x41e5cb0;
inline constexpr std::uintptr_t attach_superbundle = 0x41dfb70;
inline constexpr std::uintptr_t prepare_toc = 0x41f8ed0;

}
