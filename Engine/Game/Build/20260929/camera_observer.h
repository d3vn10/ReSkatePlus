#pragma once
#include <array>
#include <cstdint>
#include "camera_observer_rvas.h"

namespace dingosdk::game::build::v20260929::camera_observer {
// Camera callback observed by the camera diagnostics.
inline constexpr std::array<unsigned char, 24> camera_callback_prefix{
    0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0x59,0x18,0x48,0x8b,
    0x41,0x20,0xc5,0xfa,0x10,0x02,0x48,0x89,0x53,0x60,0x48,0x8d};
// Return address of the verified native caller of the camera callback.
// Camera callback object.
// The two native camera entry variants. 14178fb00 and 14178fba0 create them
// through the same 1417224a0 base constructor; the live stuck-camera capture
// selected the alternate one after a camera/editor transition.
// Callback helper.
}
