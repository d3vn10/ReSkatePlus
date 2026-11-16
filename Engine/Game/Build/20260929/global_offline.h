#pragma once
#include <array>
#include <cstdint>
#include "global_offline_rvas.h"

namespace dingosdk::game::build::v20260929::global_offline {
// Shared OnlineEnabled leaf hooked by the offline boot path (Extension/Boot/offline_boot.cpp).
// MOV AL,1; RET followed by the inspected INT3 padding.
inline constexpr std::array<unsigned char, 16> online_enabled_prefix{
    0xb0,0x01,0xc3,0xcc,0xcc,0xcc,0xcc,0xcc,
    0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc};
// Return addresses of the direct OnlineEnabled calls the offline boot path answers.
// Return address inside the reflected Flow getIsOnlineEnabled invocation adapter.
// Post-spawn progression guard.
}
