#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <array>
#include <cstdint>
#include "localization_rvas.h"

namespace dingosdk::game::build::v20260929::localization {
// The string-override store every localized-text lookup checks before the shipped string
// database (hasString 0x3320c70 and getString 0x3322fd0 both do). Retail fills its layer 0
// from the backend's client_strings_v1 service (handler 0x268a00, lambda 0x2696d0: clear
// the layer with 0x331d060, then set each record). An id found nowhere is shown as written.
// Ids are hashed h = h * 33 + byte from 0x00000000 (0x3325670).
// Address store(): the store (a static at +0x761b410).
inline constexpr std::array<unsigned char, 19> override_store_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// bool set(Address store, const char* id, const char* utf8, int layer): takes the store's
// lock, converts the text to UTF-16 and registers its glyphs. Engine allocator: game thread.
// It converts into a 0x000-byte frame buffer (rbp-0x00, capacity 0x000 in edx), so a text is
// kept within 511 UTF-16 units and its terminator.
inline constexpr std::array<unsigned char, 19> override_set_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// bool has(uint32 hash): the id is in the store or the database.
inline constexpr std::array<unsigned char, 19> string_exists_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// set() encodes the text against the loaded string database (0x3325800: UTF-8 to UTF-16,
// each character registered with the database 0x3320730, then 0x3323d90 writes the stored
// form). With no database loaded yet it stores an empty string, which shows as blank.
// The database is found as that function finds it: the qword at database_loaded must be
// set; then the first of the {u32 in use (0 = this one), pad, entry*} rows at *databases,
// indices 0..*database_last, whose entry +0x00 is the database.

// The UI's text capitalization (Rime text style: RimeTextCapitalization None, Uppercase,
// Lowercase, Title, Reverse). 0x42a21b0 maps the style to a mode and calls
// size convert(const char16_t* src, size_t length, char16_t* dst, size_t capacity, int mode,
//              const char* locale)
// mode 0 copies, 1 lowercases, 2 uppercases, 3 title-cases (0x54a6f90: each word's first
// character through convert with mode 2), 4 swaps case (each character through convert with
// mode 2 or 1). Modes 1 and 2 write dst[k] for src[k], one to one, while k is within capacity
// (less one for a terminator when locale is null or empty, which they then add). The locale is
// otherwise unused: the mapping has no language rules, so i uppercases to I and I lowercases to
// i. A..Z / a..z are mapped in code, the rest below 0x000 by the tables (0 = unchanged).

inline constexpr int case_lower = 1;
inline constexpr int case_upper = 2;
// char16_t tables indexed by code point below 0x000: upper[0x000] (dotless i) is 0x00,
// lower[0x000] (dotted capital I) is 0x00.
} // namespace dingosdk::game::build::v20260929::localization
