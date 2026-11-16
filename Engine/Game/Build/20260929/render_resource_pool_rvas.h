#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::render_resource_pool {
inline constexpr std::uintptr_t constructor_rva = 0x03e7f300;
inline constexpr std::uintptr_t state_owner_call_rva = 0x03f33a46;
inline constexpr std::uintptr_t index_table_constructor_rva = 0x03e7e400;
inline constexpr std::uintptr_t resource_flags_priority_rva = 0x03f56073;
inline constexpr std::uintptr_t resource_flags_visibility_rva = 0x03f5789a;

}
