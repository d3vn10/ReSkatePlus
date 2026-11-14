#pragma once

#if defined(DINGOSDK_DEV_TOOLS)

#include <cstdint>
#include <span>
#include <string_view>

namespace dingosdk::game::usage_trace
{
    // Small, explicitly selected state samples around a native game call. These
    // are observations, not a claim that every downstream effect is attributable.
    struct StateSnapshot
    {
        std::uint32_t client_state{};
        std::uint32_t game_type{};
        std::uint32_t pending_requests{};
        std::uint32_t descriptions{};
        bool valid{};
    };

    // Writes one JSONL event to RvaUsage.jsonl in RESKATE_LOG_DIRECTORY.
    // Arguments are recorded as scalar/pointer values only; pointed-to memory is
    // never read or dumped by this facility.
    // Counts hot paths without writing one event per frame. Emits a first-hit event
    // and a five-second interval/total summary.
    void record_hit(std::uintptr_t image_base,
                    std::uintptr_t rva,
                    std::string_view symbol,
                    std::span<const std::uint64_t> arguments) noexcept;

    void record_call(std::string_view phase,
                     std::uintptr_t image_base,
                     std::uintptr_t rva,
                     std::string_view symbol,
                     std::span<const std::uint64_t> arguments,
                     const StateSnapshot *before,
                     const StateSnapshot *after,
                     std::uint64_t elapsed_ms = 0) noexcept;
}

#endif
