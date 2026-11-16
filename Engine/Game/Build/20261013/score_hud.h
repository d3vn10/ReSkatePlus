#pragma once
#include <cstdint>
#include <string_view>

// The SDK's display-only score HUD model, mapped by read-only HUD probes on this build and checked
// by drawing an owned model on 2026-11-06. These are field-name hashes from the game's type data;
// feature code finds each offset through that type before reading or writing its owned model.
namespace dingosdk::game::build::v20260929::score_hud {
inline constexpr std::uint16_t score_view_size = 896;
inline constexpr std::string_view score_widget = "UI/Features/HUD/Scoring/ScoringHUD_Widget";
inline constexpr std::string_view trick_row_widget = "UI/Features/HUD/Scoring/TrickHistoryStackItem";

// The root's line points before its multiplier, its ring value, and its action/timer signals.
inline constexpr std::uint32_t line_score = 0x00000000;
inline constexpr std::uint32_t multiplier = 0x00000000;
inline constexpr std::uint32_t action_active = 0x00000000;
inline constexpr std::uint32_t line_timer = 0x00000000;
inline constexpr std::uint32_t is_slam = 0x00000000;
inline constexpr std::uint32_t sequence_header = 0x00000000, sequence_descriptor = 0x00000000;

// LINE is driven by the widget from LineScore and stays for its summary after the action signals
// end. We read it to yield to the game's summary; the owned model writes LineScore, never LINE.
inline constexpr std::uint32_t line_shown = 0x00000000;
inline constexpr std::uint32_t action_points = 0x00000000, multiplier_text = 0x00000000;
inline constexpr std::uint32_t action_fill = 0x00000000;
inline constexpr float action_fill_points = 400;
inline constexpr std::uint32_t line_on = 0x00000000, ring_on = 0x00000000;
inline constexpr std::int32_t extra_info_line_score = 0;

// A copy clears both stacks and keeps the live scoring-data ref at its new model's zero default.
// Rows get newly created models; no game row or its back-reference is copied.
inline constexpr std::uint32_t scoring_data = 0x00000000;
inline constexpr std::uint32_t trick_list = 0x00000000, second_stack = 0x00000000;
inline constexpr std::uint32_t trick_row_schema = 0x00000000;
inline constexpr std::uint16_t trick_row_size = 2048;
inline constexpr std::uint32_t row_name = 0x00000000;
inline constexpr std::uint32_t label_text = 0x00000000; // LabelViewModel's Text is a char* (kind 7).
}
