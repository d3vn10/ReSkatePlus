#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include "physics_tuning_rvas.h"

namespace dingosdk::game::build::v20261013::physics_tuning {
// Gameplay/SkatePhysicsTuning (SkatePhysicsTuningAsset, EBX partition
// ca6b5fd8-323d-4f5c-9eb0-e6709f4bbfc2 in win32/levels/game/bam_levelroot/bam_levelroot): one
// shared instance per process, read by every skater's physics. SkatePhysicsTuningEntity
// (client realm) publishes it in this global through its only writer, `mov [global], rcx; ret`.

// The instance: +8 its type info, +0x00 Name (char*). Groups sit at their EBX dataOffsets
// (Footplant 0x00, Trucks 0x000, Jump 0x0000, Mode 0x0000, Deck 0x0000, Wheels 0x0000).

} // namespace dingosdk::game::build::v20261013::physics_tuning
