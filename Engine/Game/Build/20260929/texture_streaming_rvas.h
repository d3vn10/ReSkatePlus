#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::texture_streaming {
inline constexpr std::uintptr_t manager = 0x76e92e0; // TextureStreamingManager*, null until made
inline constexpr std::uintptr_t stack_check_failed = 0x546be9c;

}
