#pragma once

#if defined(DINGOSDK_DEV_TOOLS)

#include <cstddef>
#include <cstdint>
#include <atomic>
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

    // Writes JSON Lines events to a unique RvaUsageN.json file in RESKATE_LOG_DIRECTORY.
    // Arguments are recorded as scalar/pointer values only; pointed-to memory is
    // never read or dumped by this facility.
    // Each instrumented call site owns its counters. Hot paths emit a first-hit event
    // and five-second interval/total summaries without writing one event per call.
    struct HitCounter
    {
        std::atomic<std::uint64_t> first{}, total_calls{}, interval_calls{}, next_report{};
    };

    void record_hit(HitCounter &counter,
                    std::uintptr_t image_base,
                    std::uintptr_t rva,
                    std::string_view symbol,
                    std::span<const std::uint64_t> arguments) noexcept;

    // Records hook/patch lifecycle operations. Addresses are absolute process
    // addresses; related_address is a replacement/destination when applicable.
    void record_lifecycle(std::string_view subsystem,
                          std::string_view action,
                          std::string_view name,
                          std::uintptr_t address,
                          std::uintptr_t related_address,
                          std::size_t size,
                          std::int64_t status) noexcept;

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
