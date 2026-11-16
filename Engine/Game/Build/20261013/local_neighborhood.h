#pragma once
#include <cstdint>
#include "local_neighborhood_rvas.h"

// Neighborhood (district) progression records. The native response conversion
// 140346f80 hashes ProgressionId, then 14065c280 copies that hash into the base
// record (+0x00).
namespace dingosdk::game::build::v20260929::local_neighborhood {
// Neighborhood record model TypeInfo.
// Reflected neighborhood record type: uint32 name hash (0x00000000) and uint16 size.
// Reflected field records: Bool at +0x00 (hash 0x00000000) and the UInt32
// progression hash at +0x00 (hash 0x00000000).
}
