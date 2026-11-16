#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::effects {
inline constexpr std::uintptr_t service=0x74deb20;
inline constexpr std::uintptr_t asset_compartment = 0x16;
inline constexpr std::array<std::uintptr_t, 4> service_methods{0x10, 0x20, 0x28, 0x30};

}
