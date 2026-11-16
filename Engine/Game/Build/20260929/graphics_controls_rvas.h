#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::graphics_controls {
inline constexpr std::uintptr_t video_filter_pass_vtable = 0x60ca0d8;
inline constexpr std::uintptr_t video_filter_module_vtable = 0x60ca020;
inline constexpr std::uintptr_t filmic_vtable = 0x67d57f8;
inline constexpr std::uintptr_t filmic_data_type = 0x78c6968;
inline constexpr std::uintptr_t post_process_settings_type = 0x76f54e8;
inline constexpr std::uintptr_t film_grain_enable_field = 0x8407cb8;
inline constexpr std::uintptr_t vignette_enable_field = 0x8407b80;
inline constexpr std::uintptr_t ddc_dirty_version_field = 0x8408720;

}
