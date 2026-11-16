#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::no_bail {
inline constexpr std::uintptr_t reset_bail_causes_rva = 0x474cca0;
inline constexpr std::uintptr_t bail_core_vtable = 0x6176d30;
inline constexpr std::uintptr_t recovery_predicate = 0x47d1100;

}
