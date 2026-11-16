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
inline constexpr std::uintptr_t mesh_cull_tree_enabled = 0x95d;
inline constexpr std::uintptr_t job_wake = 0x3228400;
inline constexpr std::uintptr_t job_lockfree_push = 0x11eb850;
inline constexpr std::uintptr_t job_execute = 0x321ed00;
inline constexpr std::uintptr_t job_scheduler_immediate = 0x271;
inline constexpr std::uintptr_t job_scheduler_queue = 0x10;
inline constexpr std::uintptr_t job_scheduler_inline = 0x2c0;
inline constexpr std::uintptr_t job_scheduler_waiters = 0x2a8;
inline constexpr std::uintptr_t job_link = 0x40;
inline constexpr std::uintptr_t worker_table = 0x7607770;
inline constexpr std::uintptr_t worker_stride = 0x178;
inline constexpr std::uintptr_t worker_state = 0x5c;
inline constexpr std::uintptr_t tls_index = 0x78d40e0;
inline constexpr std::uintptr_t tls_worker = 0x6c8;
inline constexpr std::uintptr_t main_sleep_return = 0x4645328;
inline constexpr std::uintptr_t sleep_timer_handle = 0x8;
inline constexpr std::uintptr_t sleep_abort = 0x10;
inline constexpr std::uintptr_t section_list_begin = 0x128;
inline constexpr std::uintptr_t section_list_end = 0x130;
inline constexpr std::uintptr_t section_dirty = 0x18;            // dword
inline constexpr std::uintptr_t section_list_dirty = 0x1c;       // word
inline constexpr std::uintptr_t task_queue_runner_exit_rva = 0x3225d98;
inline constexpr std::uintptr_t task_queue_runner_exit_patch_offset = 16;
inline constexpr std::uintptr_t server_step_reader_lock_rva = 0x4db7ae9;
inline constexpr std::uintptr_t server_step_reader_lock_patch_offset = 22;

}
