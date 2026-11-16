#pragma once
#include <array>
#include <cstdint>
#include "initfs_rvas.h"

namespace dingosdk::game::build::v20260929::initfs {
// 16-byte InitFS AES key in the executable image.
// Code in the InitFS decryptor that references the key.
inline constexpr std::array<unsigned char, 18> decrypt_reference_prefix{
    0xc5,0xf8,0x10,0x05,0xe7,0x2b,0xfd,0x02,0x8b,0x45,0x18,
    0x48,0x8d,0x0d,0xdd,0x2b,0xfd,0x02};
}
