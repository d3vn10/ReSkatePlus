#pragma once
#include <cstdint>
#include "local_challenge_rvas.h"

// Local challenge catalog and completion delivery. Related native code: the
// real activity catalog's anonymous model allocation 1417e18c0; progress
// hydration 140735900 (uses GetModelField); OnAmpActivityEndRecieved_Global
// sender 1406de170 (rejects an empty EID); 1415b4d90 places a graph's page
// table after its aligned frame size; the criterion value copy constructor
// 14591b5e0 treats its array fields as reference-counted ValueRefs.
namespace dingosdk::game::build::v20260929::local_challenge {
// Challenge (activity) owner system.
// Pointer to the game context that owns the challenge system.
// uint32 registered offset of the challenge owner in that context.
// uint16 reflected sizes of ChallengeCounts and ActivityData.
// Reflected ActivityData criterion-group field record (hash 0x571954a7, +0x30, UInt64).
// Model TypeInfos of the challenge catalog.
// ActivityListUpdated event type.
}
