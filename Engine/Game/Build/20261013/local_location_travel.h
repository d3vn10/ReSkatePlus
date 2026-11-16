#pragma once
#include <array>
#include <cstdint>
#include "local_location_travel_rvas.h"

// Offline location travel catalog. The owner pointer is engine::location_owner;
// the native hooks are in location_travel.
namespace dingosdk::game::build::v20260929::local_location_travel {
// Location travel owner vtable.
// Data model TypeInfos of a travel location and an access point.
// Event types dispatched once the catalogs are published.
// Type records (uint32 hash at +0, uint16 size at +6) of the 0x00-byte access
// point, the 0x00-byte location and two 1-byte types.
}
