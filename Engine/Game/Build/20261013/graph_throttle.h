#pragma once
#include "engine.h"
#include <cstdint>

namespace dingosdk::game::build::v20260929::graph_throttle {
// What a loaded graph's delayed re-post slots are bound to before the throttle
// takes them over.
inline constexpr std::uintptr_t post_seconds_fast = engine::post_event_delayed_seconds;
inline constexpr std::uintptr_t post_frames_fast = engine::post_event_delayed_frames;

// The two re-post natives' hashes in a graph's native call table.
//   PostEventDelayedSeconds 0x00000000: self, event, port offset, payload, float seconds.
//   PostEventDelayedFrames  0x00000000: self, payload, port offset, payload type, int frames.
inline constexpr std::uint32_t post_seconds_hash = 0x00000000, post_frames_hash = 0x00000000;

// Each graph as shipped (read from a res_all dump; the feature checks every one of these values
// before it patches, so a graph that differs is refused): header +0x00 frame bytes, +0x00
// constant bytes, +0x00 code words; code at code_offset with its first word; the native
// call table of {hash, pc} pairs. The graph calls `native` `sites` times; the one patched is
// at `pc` (word 0 = next << 8 | 4: four operands and the delay), with S0 as its first operand
// and the zero delay constant K`delay` as its fifth.
struct Repost {
    std::uint32_t hash;
    const char* name;
    std::uint32_t frame, constants, code_words, code_offset, first_word, table_offset, table_count;
    std::uint32_t native, sites, pc, next, delay;
};
// Re-posts at pc 0x00, then GetBoolSetting("CommunityEvent.DebugDrawQueues") (a retail stub:
// its default, false) sends it to RETURN.
inline constexpr Repost coordinator_debug_draw{0x00000000, "MPActivityCoordinator.OnDebugDrawUpdate", 0x00, 0x000, 0x000,
    0x000, 0x000, 0x000, 48, post_seconds_hash, 1, 0x00, 0x00, 0x00};
// Re-posts at pc 0x00, then returns while field +0x00 is clear (every traced run).
inline constexpr Repost input_visualization{0x00000000, "InputVisualization_Dev.OnUpdate", 0x00, 0x000, 0x000, 0x000,
    0x000, 0x000, 63, post_seconds_hash, 1, 0x00, 0x00, 0x00};
// Re-posts at pc 0x00; its per-entry print loop needs field +0x000 and the retail-stubbed
// "DingoActivity.DrawActivityDebug".
inline constexpr Repost leaderboard_debug_print{0x00000000, "ActivityLeaderboardManager.OnDebugPrintLoop", 0x00, 0x000,
    0x000, 0x000, 0x000, 0x000, 49, post_seconds_hash, 1, 0x00, 0x00, 0x000};
// Play Event failed (pc 0x00 -> 0x0000): logs "…retrying for {0}" and re-posts the
// activation at pc 0x0000. Its other re-post (pc 0x000, success path) is another call.
inline constexpr Repost narrative_play_retry{0x00000000, "NarrativeSet Play Event retry", 0x00, 0x000, 0x000, 0x000,
    0x0000, 0x0000, 167, post_seconds_hash, 2, 0x0000, 0x0000, 0x000};
// Not IsOnline && IsSessionReady (pc 0x00 -> 0x000): asks again at pc 0x000.
inline constexpr Repost quick_drop_entitlements{0x00000000, "QuickDropPersistenceManager.OnWaitForEntitlements", 0x00,
    0x000, 0x000, 0x000, 0x000, 0x000, 52, post_seconds_hash, 1, 0x000, 0x000, 0x000};
// Not ready (pc 0x000 -> 0x000): asks again at pc 0x000. Its ready re-post (pc 0x000) is another call.
inline constexpr Repost milestone_init{0x00000000, "MilestoneManager.OnInitLoop", 0x00, 0x000, 0x000, 0x000, 0x000,
    0x000, 26, post_seconds_hash, 2, 0x000, 0x000, 0x00};
// Not IsOnline && quests present (pc 0x00 -> 0x00): asks again next frame at pc 0x00. Its
// ready branch (pc 0x00, two frames) is another call.
inline constexpr Repost quest_delayed_init{0x00000000, "QuestManager.OnDelayedInit", 0x00, 0x00, 0x00, 0x00, 0x000,
    0x000, 5, post_frames_hash, 2, 0x00, 0x00, 0x00};
}
