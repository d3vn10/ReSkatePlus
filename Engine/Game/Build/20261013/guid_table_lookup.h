#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
// find(table, key, a, b): an open-addressed table of u16 record indices (+0x000, sized from the
// record count at +0x00, at least 16 and 1.5 times it, rounded up to a power of two), probed from
// the key's first dword by (key & 0x00) | 1. Each index is turned into its record's 16-byte key
// by a virtual call (+0x00) on the source at +0x00. The probe stops only on a matching key: a key
// that is not there loops forever, the empty slots (0x0000) included. That froze the game joining
// custom-map servers with the Throwdown asset pack installed (key 53c67866-6592-9274-105c-
// eb835e72f8c9, in two hang dumps, 2026-10-17). On a match it goes on to record(table, index, a, b).
#include "guid_table_lookup_rvas.h"

namespace dingosdk::game::build::v20261013::guid_table_lookup {


inline constexpr std::uintptr_t record = record_contract.rva;
inline constexpr std::uintptr_t count = 0x00, slots = 0x000, source = 0x00, key_of = 0x00;
inline constexpr std::uint16_t empty = 0x0000;
}
