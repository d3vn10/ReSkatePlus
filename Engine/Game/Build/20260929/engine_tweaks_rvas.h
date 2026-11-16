#pragma once
#include <cstdint>
#include <array>

// September 29 2026 executable RVAs.
namespace dingosdk::game::build::v20260929::engine_tweaks {
inline constexpr std::uintptr_t gi_update_frame_return = 0x51966f4;
inline constexpr std::uintptr_t gi_update_loop_return = 0x519660c;
inline constexpr std::uintptr_t settings_lookup = 0x18d6ef0;
inline constexpr std::uintptr_t settings_manager = 0x7482ce0;
inline constexpr std::uintptr_t world_render_settings_type = 0x78adf88;
inline constexpr std::uintptr_t job_wake = 0x3228400;
inline constexpr std::uintptr_t job_lockfree_push = 0x11eb850;
inline constexpr std::uintptr_t job_execute = 0x321ed00;
inline constexpr std::uintptr_t worker_table = 0x7607770;
inline constexpr std::uintptr_t tls_index = 0x78d40e0;
inline constexpr std::uintptr_t main_sleep_return = 0x4645328;
inline constexpr std::uintptr_t task_queue_runner_exit_rva = 0x3225d98;
inline constexpr std::uintptr_t server_step_reader_lock_rva = 0x4db7ae9;

}
