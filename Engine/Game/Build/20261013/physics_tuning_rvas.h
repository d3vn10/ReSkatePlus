#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::physics_tuning {
inline constexpr std::uintptr_t asset_global = 0x7785dc0;
inline constexpr std::uintptr_t asset_type_info = 0x77ab1e0;

inline constexpr Fingerprint asset_setter_contract{0x47619c0, {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

inline constexpr std::uintptr_t asset_type_offset = 0x0, asset_name_offset = 0x00, asset_fields_start = 0x00;
inline constexpr std::uint32_t asset_size = 0x0000;
// FloatCurve (0x00 bytes): Points (pointer to the first element; u32 count at -4, masked
// 0x00000000) at +0x00, MinX +0x00, MaxX +0x00; FloatCurvePoint is 0x00 bytes. Curve
// pointers in the asset may carry a tag in bit 2.
inline constexpr std::uintptr_t curve_points_offset = 0x00, curve_min_offset = 0x00, curve_max_offset = 0x00;
inline constexpr std::uint32_t curve_point_size = 0x00;
// About 60 fields (Feet, Jump, Steering, Manual, Slide, Push, Friction.SlideVsNormal,
// Wheels.Softest*) are copied into a heap block (0x000 bytes) of each skater's physics
// controller when it is created: the controller is the skater component's physics core
// (component +0x00, bail_core_vtable), and its +0x000 points at the block. void
// refresh(block) copies them again from the asset (it reads only the asset and static
// globals, and assigns the block's curve copies, so it may run again on a live block).
inline constexpr Fingerprint refresh_block_contract{0x4750f30, {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
}
