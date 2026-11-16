#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::media_catalog {
inline constexpr std::uintptr_t services_vtable = 0x6070f98;
inline constexpr std::uintptr_t container_vtable = 0x60cd120;
inline constexpr std::uintptr_t listener_vtable = 0x60cd140;
inline constexpr std::uintptr_t container_listener = 0x60;
inline constexpr std::uintptr_t container_catalogue = 0x68;
inline constexpr std::uintptr_t catalogue_folder = 0x0;
inline constexpr std::uintptr_t catalogue_entries = 0x38;
inline constexpr std::uintptr_t catalogue_entry_size = 0x130;
inline constexpr std::uintptr_t catalogue_ready = 0x5c;

}
