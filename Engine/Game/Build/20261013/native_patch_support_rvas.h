#pragma once
#include <cstdint>

// September 29 2026 executable RVAs for the native patch contract.
namespace dingosdk::game::build::v20260929::native_patch_support {
inline constexpr std::uintptr_t verifier = 0x00000000;
inline constexpr std::uintptr_t signature_import = 0x00000000;
inline constexpr std::uintptr_t signature_return = 0x00000000;
inline constexpr std::uintptr_t crypto_thunk_rva = 0x00000000;
inline constexpr std::uintptr_t reader_call_rva = 0x00000000;
inline constexpr std::uintptr_t layout_call_rva = 0x00000000;
inline constexpr std::uintptr_t unwind_root_rva = 0x00000000;
inline constexpr std::uintptr_t unwind_middle_rva = 0x00000000;
inline constexpr std::uintptr_t unwind_return_rva = 0x00000000;
}
