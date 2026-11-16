#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::native_indicators {
inline constexpr std::uintptr_t hud_tick = 0x1aec120;
inline constexpr std::uintptr_t provider_remove = 0x1aebd90;
inline constexpr std::uintptr_t widget_create = 0x4379f80;
inline constexpr std::uintptr_t indicator_register = 0x1ae76a0;
inline constexpr std::uintptr_t presenter_flush = 0x1ae7920;
inline constexpr std::uintptr_t property_bind = 0x2b7e930;
inline constexpr std::uintptr_t class_reference_getter = 0x3dd1890;
inline constexpr std::uintptr_t presenter_registry = 0x71267e0;
inline constexpr std::uintptr_t hud_presenter_vtable = 0x629ea70;
inline constexpr std::uintptr_t indicator_source_vtable = 0x629f0e0;
inline constexpr std::uintptr_t widget_parent_vtable = 0x6388c90;
inline constexpr std::uintptr_t compass_icon_type = 0x76e2850;
inline constexpr std::uintptr_t vec3_type = 0x765d350;
inline constexpr std::uintptr_t user_info_type = 0x7258fb0;

}
