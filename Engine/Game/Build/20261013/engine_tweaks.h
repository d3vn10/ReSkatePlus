#pragma once
#include "Engine/Game/Build/fingerprint.h"
#include <array>
#include <cstdint>
#include "engine_tweaks_rvas.h"

// Engine work the developers left on the table (Extension/Rendering/engine_tweaks.cpp, `perf`
// toggles; analysis/sim-performance.md "Engine code").
namespace dingosdk::game::build::v20260929::engine_tweaks {
// 1. Enlighten GI's per-frame update(manager, mode): runs on the render thread every rendered
// frame and dispatches the probe-set solves (SolveEntireProbeSetTask 0x54d5090, SolveIrradianceTask
// 0x54c7350). Its own skip path (manager + 0x00 bits 0/1) returns at once with nothing done.
// The GI frame (0x5196270) calls it once a frame (gi_update_frame_return), or, while a setting
// (+0x000 of the object its +0x000 getter returns) is on, settings + 0x00 times in a row
// (gi_update_loop_return).


// 2. meshCull's scheduler, run once per frame before the cull jobs; 11 arguments (rcx, rdx, r9
// and stack slots 5, 6 and 11 are read; no floating-point ones). It and the job (0x4eecaa0)
// each read WorldRenderSettings.MeshCullTreeEnabled (+0x000): 0 = flat path testing every chunk
// against every view, 1 = octree pre-cull (meshCullPrepare 0x4eb2b70). The settings come from
// settings_lookup(*settings_manager, world_render_settings_type).


// 3. Job wake-ups. push(scheduler, job): if scheduler + 0x000 runs the job at once (execute
// 0x321ed00 on scheduler + 0x000); else pushes it (lockfree_push(scheduler + 0x00, job, job + 0x00))
// and wakes the first worker, in bit order, of the waiter mask (scheduler + 0x000) that wake()
// accepts. Worker states (worker_table + i * worker_stride + worker_state): 3 running, 4 entering
// idle (wake = one CAS 4 -> 3), 5 asleep (wake = CAS 5 -> 3 and a semaphore release). The idle
// wait (0x3228220) sets 3 -> 4, calls register_idle (no arguments), which puts the worker's bit in
// its schedulers' and queues' masks and returns non-zero if work was already there; on 0 it sets
// 4 -> 5 and sleeps, or, when a pusher got there first (state 3), unregisters and runs again.
// The worker is read from the thread's TLS block (gs:[0x00][*tls_index] + tls_worker).



// 4. Precise sleep(timer, nanoseconds): an auto-reset, non-high-resolution waitable timer (timer +
// 8, made by 0x18b0220) for all but the last 1 ms (0x73c7ba0, from timeGetDevCaps), then a spin,
// at thread priority 15 while it waits; clears and obeys an abort flag (timer + 0x00) on every
// pass. The abort (0x18bd9d0) sets the flag and fires the timer at once. The main loop (0x4645080)
// calls it with what is left of its 10 ms budget; main_sleep_return is that call's return address.


// 5. Expression shader section dirty-bit clear(section), every frame for every section (only
// caller 0x505a9a0): takes a byte spinlock (+0x00) to clear the dirty bits -- bits 0-17 of the
// dword at +0x00 when the list at +0x000..+0x000 is empty, else bits 0-5 of the word at +0x00 --
// and only does more (sets bit 18, notifies) when some were set. With none set it writes back
// the same values: no effect but the lock.

inline constexpr std::uint32_t section_dirty_mask = 0x00000;
inline constexpr std::uint16_t section_list_dirty_mask = 0x00;

// 6. Simulation stall guards under Wine on macOS (Rosetta 2 + hybrid P/E cores):
// (a) TaskQueue::runnerLoop (0x3225d00): when a runner sees an empty ring buffer (+0x00) and
// decrements the active runner count (+0x000) via `lock xaddl [rax+0x000], -1`, it only re-reads
// the ring buffer after the barrier when the old runner count was 1 (`cmp ecx, 1; jne exit`).
// When two runners exit concurrently while EntityGraph pushes tasks (0x184d630, which skips
// spawning a runner whenever +0x000 != 0), skipping the post-barrier check strands tasks in the
// queue with 0 active runners and stalls GameSimulationLoop (GameStall 0x18e13d2). Replacing the
// 6-byte `jne` at +16 with a 6-byte nop makes every exiting runner re-check +0x00 after `xaddl`.
inline constexpr std::array<unsigned char, 32> task_queue_runner_exit_contract{
    0x00,0x00,0x00,                          // mov rax,[rbx]
    0x00,0x00,                               // mov ecx,edi (-1)
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // lock xadd [rax+0x000],ecx
    0x00,0x00,0x00,                          // cmp ecx,1
    0x00,0x00,0x00,0x00,0x00,0x00,           // jne exit (0x3225f44)
    0x00,0x00,0x00,                          // mov rax,[rbx]
    0x00,0x00,0x00,0x00,0x00,0x00,           // mov ecx,[rax+0x00]
    0x00};                                   // nop
inline constexpr std::array<unsigned char, 6> task_queue_runner_exit_patch{
    0x00,0x00,0x00,0x00,0x00,0x00};          // 6-byte nop

// (b) Server step shared reader lock (0x4db7ad0): tests `eax & 0x00000000` (writer active/waiting)
// at 0x4db7ae9 and then executes a single `lock cmpxchg [rcx+0x00000], edx` without a retry loop.
// If two readers race on +0x00000 while no writer holds the lock, the losing reader jumps to the
// writer-wait path (0x4db7bad), which queues a retry job on +0x00000; with no writer active
// (+0x00000 == 0), 0x321e620 returns without scheduling the retry job and leaks the Server step
// continuation token. Retargeting the 6-byte `jne` at +22 back to +0 (`jne 0x4db7ae9`) retries
// the reader CAS until it succeeds or a writer sets the upper 16 bits.
inline constexpr std::array<unsigned char, 28> server_step_reader_lock_contract{
    0x00,0x00,0x00,0x00,0x00,                // retry: test eax,0x00000000
    0x00,0x00,0x00,0x00,0x00,0x00,           // jne wait_for_writer (0x4db7bad)
    0x00,0x00,0x00,                          // lea edx,[rax+1]
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // lock cmpxchg [rcx+0x00000],edx
    0x00,0x00,0x00,0x00,0x00,0x00};          // jne wait_for_writer (0x4db7bad)
inline constexpr std::array<unsigned char, 6> server_step_reader_lock_patch{
    0x00,0x00,                               // jne retry (0x4db7ae9)
    0x00,0x00,0x00,0x00};                    // 4-byte nop
static_assert(static_cast<signed char>(server_step_reader_lock_patch[1]) ==
              0 - static_cast<int>(server_step_reader_lock_patch_offset + 2));
inline constexpr std::uintptr_t mesh_cull_tree_enabled = 0x000;
inline constexpr std::uintptr_t job_scheduler_immediate = 0x000;
inline constexpr std::uintptr_t job_scheduler_queue = 0x00;
inline constexpr std::uintptr_t job_scheduler_inline = 0x000;
inline constexpr std::uintptr_t job_scheduler_waiters = 0x000;
inline constexpr std::uintptr_t job_link = 0x00;
inline constexpr std::uintptr_t worker_stride = 0x000;
inline constexpr std::uintptr_t worker_state = 0x00;
inline constexpr std::uintptr_t tls_worker = 0x000;
inline constexpr std::uintptr_t sleep_timer_handle = 0x0;
inline constexpr std::uintptr_t sleep_abort = 0x00;
inline constexpr std::uintptr_t section_list_begin = 0x000;
inline constexpr std::uintptr_t section_list_end = 0x000;
inline constexpr std::uintptr_t section_dirty = 0x00;            // dword
inline constexpr std::uintptr_t section_list_dirty = 0x00;       // word
inline constexpr std::uintptr_t task_queue_runner_exit_patch_offset = 16;
inline constexpr std::uintptr_t server_step_reader_lock_patch_offset = 22;

}
