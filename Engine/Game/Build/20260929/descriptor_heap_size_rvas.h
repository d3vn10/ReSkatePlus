#pragma once
#include <cstdint>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::descriptor_heap_size {
inline constexpr std::uintptr_t total_cap_rva = 0x0410eaa0;
inline constexpr std::uintptr_t cap_load_rva = 0x0410eaee;
inline constexpr std::uintptr_t layout_rva = 0x0410eabb;
inline constexpr std::uintptr_t heap_constructor_rva = 0x040e1f00;
inline constexpr std::uintptr_t begin_frame_rva = 0x040e2520;

}
