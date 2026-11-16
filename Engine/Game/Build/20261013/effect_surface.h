#pragma once
#include "park_editor.h"
#include "effect_surface_rvas.h"

namespace dingosdk::game::build::v20261013::effect_surface {
// Same executable as decals.h. Paired position/quaternion readers are used
// by 0x1a4ae60 and 0x3d04c80. Input: 16-byte physics world/index/generation
// handle. Output: aligned float4, quaternion in XYZW order.
inline constexpr std::uintptr_t scope_pop_slot = 0x0, result_fractions = 0x0;

}
