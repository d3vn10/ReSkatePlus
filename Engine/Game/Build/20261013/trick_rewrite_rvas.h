#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::trick_rewrite {
inline constexpr std::uintptr_t record_trick = 0x4798f00;
inline constexpr std::uintptr_t resolve_localized_key = 0x2e1ac10;
inline constexpr std::array<std::uintptr_t, 3> ticker_label_route{0x758078, 0x59c1241, 0x16d2178};

}
