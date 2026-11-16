#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::loose_files {
inline constexpr std::uintptr_t vfs_open = 0x4b51130;
inline constexpr std::uintptr_t vfs_exists = 0x4b52660;
inline constexpr std::uintptr_t create_directory = 0x4b4c220;
inline constexpr std::uintptr_t vfs_mount = 0x4b53f90;
inline constexpr std::uintptr_t global_vfs = 0x4b4d410;
inline constexpr std::uintptr_t mount_reference = 0x4b53e62;

}
