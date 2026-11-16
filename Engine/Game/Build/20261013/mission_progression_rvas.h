#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::mission_progression {
inline constexpr std::uintptr_t quest_lookup = 0x825750;
inline constexpr std::uintptr_t component_query = 0x277e2b0;
inline constexpr std::uintptr_t quest_status_query_call = 0x82cbb9;
inline constexpr std::uintptr_t quest_status_query_return = 0x82cbd6;
inline constexpr std::uintptr_t quest_status_component_descriptor = 0x722b370;
inline constexpr std::uintptr_t quest_expression_call = 0x83508c;
inline constexpr std::uintptr_t quest_expression_return = 0x8350a5;
inline constexpr std::uintptr_t query_context_initializer = 0x276b590;
inline constexpr std::uintptr_t query_result_vtable = 0x60de9c8;
inline constexpr std::uintptr_t quest_status_type = 0x722b210;

}
