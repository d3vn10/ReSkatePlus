#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::decals {
inline constexpr std::uintptr_t create=0x1987220;
inline constexpr std::uintptr_t release=0x19862f0;
inline constexpr std::uintptr_t properties_init=0x5b09bb0;
inline constexpr std::uintptr_t settings_init=0x5b09c70;
inline constexpr std::uintptr_t reference_assign=0x3dd1d70;
inline constexpr std::uintptr_t reference_release=0x3dc5e90;
inline constexpr std::uintptr_t manager_create=0x1991bf0;
inline constexpr std::uintptr_t manager_release=0x19925c0;
inline constexpr std::uintptr_t resource_release=0x48a9f80;
inline constexpr std::uintptr_t current_context=0x18b6ae0;
inline constexpr std::uintptr_t update_properties=0x19939a0;
inline constexpr std::uintptr_t update_transform=0x1993590;

inline constexpr std::uintptr_t manager = 0x749b1d8;
inline constexpr std::uintptr_t manager_vtable = 0x6289f70;
}
