#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::job_system {
inline constexpr std::uintptr_t spin_ticks = 0x760a670;
inline constexpr std::uintptr_t spin_milliseconds = 0x760aa10;
inline constexpr std::uintptr_t thread_count = 0x7607724;

}
