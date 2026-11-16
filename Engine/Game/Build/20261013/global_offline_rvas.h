#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::global_offline {
inline constexpr std::uintptr_t online_enabled = 0x3bbb10;
inline constexpr std::uintptr_t backend_predicate_return = 0x2830c4;
inline constexpr std::uintptr_t async_prepare_character_return = 0x7fe2f2;
inline constexpr std::uintptr_t character_readiness_return = 0x8070d5;
inline constexpr std::uintptr_t flow_adapter_return = 0x58e2abe;
inline constexpr std::uintptr_t post_spawn_progression_return = 0x82ab99;

}
