#pragma once
#include <cstdint>
#include "named_settings_rvas.h"
#include "engine.h"
#include "Engine/Game/Build/fingerprint.h"
namespace dingosdk::game::build::v20261013::named_settings {
// Named getter/setter callers at 0x2d6dfe0..0x2d6e277 and the preset
// parser at 0x5f77ef independently establish these ABIs. The typed setter
// increments DdcDirtyVersion and calls registered setting listeners.
inline constexpr std::uintptr_t named_settings_manager = engine::settings_manager;
inline constexpr std::uintptr_t native_bool = engine::bool_type;
inline constexpr std::uintptr_t native_int32 = engine::int32_type;
inline constexpr std::uintptr_t native_uint32 = engine::uint32_type;
inline constexpr std::uintptr_t native_float32 = engine::float_type;
inline constexpr std::uintptr_t native_cstring = engine::string_type;
// Pointer to the reflection symbol registry (count at +0x00); populated before
// named lookups are attempted.
// Enumeration (reads only). The manager's group map (FUN_1418d2630 registers,
// FUN_1418d2d30 walks it): buckets +0x00, bucket count u32 +0x00, element count
// u32 +0x00; node {char* name, object*, ..., next +0x00}; object +8 = class
// TypeInfo. A second map of raw structs (FUN_1418d2930): buckets +0x00, count
// +0x00, elements +0x00; node {char* name, TypeInfo*, data*, ..., next +0x00}.
// The manager lock is the critical section at +0x00.
inline constexpr std::size_t group_buckets = 0x00, group_bucket_count = 0x00, group_count = 0x00, group_next = 0x00;
inline constexpr std::size_t raw_buckets = 0x00, raw_bucket_count = 0x00, raw_count = 0x00, raw_next = 0x00;
inline constexpr std::size_t manager_lock = 0x00;
// Registry name map (FUN_143dabc00): buckets +0x00, count u32 +0x00; node
// {u64 key, char* name, next +0x00}, keyed by field-data or descriptor address.
inline constexpr std::size_t name_buckets = 0x00, name_bucket_count = 0x00;
// TypeInfo: +0 descriptor, +0x00 super TypeInfo (FUN_143dc4960). Descriptor:
// +4 u16 flags (kind = bits 5..9; 3 class, 2 value type), +0x00 u16 own field
// count, fields at +0x00 (class) or +0x00 (value type, FUN_143dd70a0); 0x00-byte
// field data {u32 name hash, u16 offset @8, TypeInfo* @0x00} (FUN_143db7ac0).

} // namespace dingosdk::game::build::v20261013::named_settings
