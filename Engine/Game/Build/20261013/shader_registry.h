#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <array>
#include <cstdint>
#include "shader_registry_rvas.h"

// The game's expression shader databases and their registry (Extension/World/shader_registry_guard.cpp).
// A level's `*_win32_shaderstate_db` resource (ExpressionShaderDatabase, handler 0x3f84cf0) holds a
// blob of 0x00-byte records; its object keeps one 0x00-byte item per record in a vector
// (begin +0x00, end +0x00) and the lookup ("MemoryInBuffer", pointer +0x00, size +0x00): u32 key
// count, then per key u64 key, u32 n, u32 record index[n]. Registering (0x3f71ac0, under the
// registry's lock, from the shader batch job 0x3f8a390) first adds, per key, the items
// `items + index * 0x00` to the registry's interned item lists (0x3f72280, no bounds check), then
// flags every item (+0x00) and records the registry (+0x00). The destructor unregisters through
// +0x00, which removes the flagged items from every list (0x3f79e20 -> 0x3f7aa10 -> 0x3f7a050).
// An index past the records, or a database destroyed before its queued registration ran, leaves
// a pointer in a list that nothing removes; the next walk of the lists (0x3f7ce90, reading
// *(item + 0x00)) crashes on it (Skate.exe+0x3f7cf33 / +0x3f7ceed / +0x20960b, mostly on custom
// map changes, 2026-10-17).
namespace dingosdk::game::build::v20260929::shader_registry {
// ExpressionShaderDatabase(this, allocator, blob, lookup, lookup size) -> this.

// ~ExpressionShaderDatabase(this, delete flags) -> this.

// register(registrar, database).

inline constexpr std::uintptr_t lookup_data = 0x00, lookup_size = 0x00, items_begin = 0x00, items_end = 0x00;
inline constexpr std::uintptr_t item_size = 0x00;

}
