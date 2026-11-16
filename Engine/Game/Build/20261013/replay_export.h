#pragma once
#include <array>
#include <cstdint>
#include "replay_export_rvas.h"

// The replay editor's video export (analysis/replay-editor.md).
namespace dingosdk::game::build::v20261013::replay_export {
// The engine movie encoder's frame submit: (encoder, pixels, pitch). The render
// thread calls it at the end of every rendered frame while a capture runs, and
// it encodes the frame, video and its share of audio, before it returns.
inline constexpr std::array<unsigned char, 24> submit_frame_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// The encoder the replay editor exports with; the engine has others.
// Encoder byte: a frame may be encoded. A world update sets it and an encoded
// frame clears it; submit_frame drops the frame it is called with while clear.

// The engine's Media Foundation H.264 encoder setup, which MP4 export goes through: (encoder) -> HRESULT.
// It builds the output media type and sets the codec up through ICodecAPI from the encoder's parameters
// below, which it only reads. Found by its REPORT_ON_FAIL strings, which name each field it reads
// ("encoderParams.kiloBitsPerSecond * 1000", "videoConfig.width").
inline constexpr std::array<unsigned char, 18> configure_h264_prefix{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Encoder ints: kilobits a second (MF_MT_AVG_BITRATE always; CODECAPI_AVEncCommonMeanBitRate in CBR);
// the rate control mode (eAVEncCommonRateControlMode: 0 CBR, 1 peak constrained VBR, 3 quality); the
// quality, 0..100, of mode 3 (CODECAPI_AVEncCommonQuality); the peak of mode 1, passed as it is
// (CODECAPI_AVEncCommonMaxBitRate). Then the frame's width and height (ints) and the frame rate (float).
inline constexpr std::uintptr_t frame_permit = 0x00;
inline constexpr std::uintptr_t h264_kilobits = 0x00;
inline constexpr std::uintptr_t h264_rate_mode = 0x00;
inline constexpr std::uintptr_t h264_quality = 0x00;
inline constexpr std::uintptr_t h264_peak = 0x00;
inline constexpr std::uintptr_t h264_width = 0x00;
inline constexpr std::uintptr_t h264_height = 0x00;
inline constexpr std::uintptr_t h264_fps = 0x00;

}
