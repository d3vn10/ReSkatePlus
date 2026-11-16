#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include "replay_recorder_rvas.h"
#include <cstdint>

// The game's rolling replay recording (Extension/Settings/replay_guard.cpp).
namespace dingosdk::game::build::v20260929::replay_recorder {
// end_chunk(recorder, force): run on a job worker as the game plays. Every recorder + 0x110
// frames, or when the stream (recorder + 0x950) is as full as recorder + 0x30 allows, it ends
// the chunk being recorded (recorder + 0xa80) with a "Tail" marker and starts the next: it has
// the stream drop old chunks (0x48f0e30), then asks it for the new chunk's head (0x48fb5a0, an
// allocation tagged "rplyFreezeClip"). It returns whether a chunk was ended (al).
// The store of the new head's first field, right after that allocation, with no check that the
// stream had room for one: an access violation writing 0x24 when it had none (crash reports of
// 2026-10-10, v2.0.3, skating on a server).
}
