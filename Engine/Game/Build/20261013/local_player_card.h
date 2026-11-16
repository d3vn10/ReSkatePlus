#pragma once
#include <cstdint>
#include "local_player_card_rvas.h"

namespace dingosdk::game::build::v20261013::local_player_card {
// Appearance (RIP Card recipe) manager, read through engine::appearance_manager.
// TypeInfo of the UIPlayerInfo presence connection-state value (field 7).
// TypeInfo of the UIPlayerInfo player network (field 10, record +0x00): the platform
// logo beside the name follows it. Values as EadpPlayerNetwork: 0 unknown (shown as EA),
// 1 EA, 2 PSN, 3 Xbox, 4 Steam, 5 Nintendo, 6 Apple, 7 Google.
inline constexpr std::uint32_t steam_network = 4;
}
