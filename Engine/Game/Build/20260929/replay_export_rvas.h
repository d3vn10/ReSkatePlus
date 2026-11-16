#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::replay_export {
inline constexpr std::uintptr_t submit_frame = 0x34f4610;
inline constexpr std::uintptr_t replay_encoder_vtable = 0x608c0b0;
inline constexpr std::uintptr_t frame_permit = 0x90;
inline constexpr std::uintptr_t configure_h264 = 0x34e6a80;
inline constexpr std::uintptr_t h264_kilobits = 0x70;
inline constexpr std::uintptr_t h264_rate_mode = 0x78;
inline constexpr std::uintptr_t h264_quality = 0x7c;
inline constexpr std::uintptr_t h264_peak = 0x80;
inline constexpr std::uintptr_t h264_width = 0x88;
inline constexpr std::uintptr_t h264_height = 0x8c;
inline constexpr std::uintptr_t h264_fps = 0x90;

}
