#pragma once
#include <cstdint>
#include "local_cosmetic_catalog_rvas.h"

// Retail OwnableData catalog bridge. Related native code: OwnableData decoder
// 141e2e350 (tag 0x00, field 23 -> rarityId +160); protobuf reader constructor
// 1420c8450; trivial CoreAllocator adapter 1401f4230; inventory manager
// constructor 14094dc10 with its normal subscriptions 1406bc560; 140695b00
// stores the borrowed OwnableData pointer at +0x000; default-owned
// subscription callback 1406a0c30 -> 1406b6870.
namespace dingosdk::game::build::v20261013::local_cosmetic_catalog {
// Shared control block vtable for a native OwnableData message.
// OwnableData (cosmetic UI) manager.
// Ownable inventory manager.
// Pointer to the OwnableData manager.
// Pointer to the ownable inventory manager.
}
