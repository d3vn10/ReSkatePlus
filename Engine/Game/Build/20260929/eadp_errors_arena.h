// Reviewed native contract for supported_build::game_sha256 (September 29 2026).
// The arena EA's error reporting (EadpErrorsData) allocates from: 80 MiB, made
// about seven seconds into startup, before the named settings can be changed.
#pragma once

#include <array>
#include <cstdint>
#include "eadp_errors_arena_rvas.h"

namespace dingosdk::game::build::v20260929::eadp_errors_arena {
// In the arena's factory (0x02318660): the load of its size from the settings
// object (EadpErrorsData.EadpErrorsDataPlatformSpecificMemPoolSize) and the
// vtable load after it, the two complete instructions moved into the stub.
//   mov eax, dword ptr [rax + 0x7c]
//   lea rcx, [rip + 0x03fffd17]        ; 0x063183d8
// No branch in the factory lands inside them; the factory goes on at resume_rva.
inline constexpr std::array<std::uint8_t, 10> size_load{
    0x8b, 0x40, 0x7c, 0x48, 0x8d, 0x0d, 0x17, 0xfd, 0xff, 0x03};
} // namespace dingosdk::game::build::v20260929::eadp_errors_arena
