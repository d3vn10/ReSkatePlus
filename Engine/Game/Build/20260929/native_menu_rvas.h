#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::native_menu {
inline constexpr std::uintptr_t action_prototype = 0x7f72df0;
inline constexpr std::uintptr_t action_invoker = 0x9769a0;
inline constexpr std::uintptr_t widget_blueprint_type = 0x77658b8;
inline constexpr std::uintptr_t asset_record_type = 0x7492f00;
inline constexpr std::uintptr_t asset_list_type = 0x7492f90;
inline constexpr std::uintptr_t texture_registry = 0x775dfe8;
inline constexpr std::uintptr_t image_asset_vtable = 0x65ade20;
inline constexpr std::uintptr_t image_asset_type = 0x7767800;
inline constexpr std::uintptr_t texture_asset_vtable = 0x65aed88;
inline constexpr std::uintptr_t texture_asset_type = 0x7768538;

inline constexpr Fingerprint model_record_load_contract{0x1918520,
    {0x48,0x89,0x5c,0x24,0x08,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,
     0x48,0x83,0xec,0x50,0x41,0xbd,0x00,0x01,0x00,0x00,0x48,0x8d,0x6a,0xf0,0x66,0x44}};
}
