#pragma once
#include <cstdint>
#include <span>
#include <array>

// September 29 2026 executable RVAs for the native patch contract.
dingosdk::game::build::v20260929::native_patch_support {
inline constexpr std::uintptr_t verifier = 0x041fb4c0;

inline constexpr std::uintptr_t signature_import = 0x05febbc8;

inline constexpr std::uintptr_t signature_return = 0x041fb5f1;

struct Contract { std::uintptr_t rva; std::span<const std::uint8_t> bytes; };
inline constexpr Contract contracts[]{
    {verifier, verifier_bytes}, {0x0546dc26, crypto_thunk},
    {0x041ed5ea, reader_call}, {0x041bd580, layout_call},
    {0x06e480e4, unwind_root}, {0x06e480f8, unwind_middle}, {0x06e4810c, unwind_return}
}
