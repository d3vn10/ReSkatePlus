#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20261013::profiler_labels {
inline constexpr std::uintptr_t client_update = 0x2e77190;
inline constexpr std::uintptr_t expression_vm = 0x16d1190;
inline constexpr std::uintptr_t expression_vm_profiled = 0x16d2010;
inline constexpr std::uintptr_t menu_expression = 0x2d39900;
inline constexpr std::uintptr_t job_run_until_done = 0x3222210;
inline constexpr std::uintptr_t job_execute = 0x321ed00;
inline constexpr std::uintptr_t job_idle_wait = 0x3228220;
inline constexpr std::uintptr_t simulation_loop_thread = 0x463c970;
inline constexpr std::uintptr_t job_worker_loop = 0x3221db0;
inline constexpr std::uintptr_t job_pop = 0x3226a30;
inline constexpr std::uintptr_t job_queue_pop = 0x321f690;
inline constexpr std::uintptr_t expression_shader_model_build = 0x505ce50;
inline constexpr std::uintptr_t render_bundle_job = 0x3f1bc40;
inline constexpr std::uintptr_t mesh_cull = 0x4eecaa0;
inline constexpr std::uintptr_t render_dispatch_end = 0x3f053f0;
inline constexpr std::uintptr_t input_dispatch = 0x31ff690;
inline constexpr std::uintptr_t ui_pointer_update = 0x31fdbf0;
inline constexpr std::uintptr_t ui_view_pointer = 0x4323f50;

inline constexpr Fingerprint client_frame_job{0x2e77770, {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
}
