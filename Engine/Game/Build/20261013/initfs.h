#pragma once
#include <array>
#include <cstdint>
#include "initfs_rvas.h"

namespace dingosdk::game::build::v20261013::initfs {
// 16-byte InitFS AES key in the executable image.
// Code in the InitFS decryptor that references the key.
inline constexpr std::array<unsigned char, 18> decrypt_reference_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00};
}
