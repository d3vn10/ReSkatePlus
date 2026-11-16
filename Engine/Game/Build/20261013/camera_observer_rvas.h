#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::camera_observer {
inline constexpr std::uintptr_t camera_callback = 0x178f5b0;
inline constexpr std::uintptr_t native_caller_return = 0x1728bd5;
inline constexpr std::uintptr_t callback_vtable = 0x625b768;
inline constexpr std::uintptr_t entry_vtable = 0x625c3d8;
inline constexpr std::uintptr_t alternate_entry_vtable = 0x625c270;
inline constexpr std::uintptr_t helper_vtable = 0x625b7b0;

}
