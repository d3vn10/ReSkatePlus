#include "usage_trace.h"

#if defined(DINGOSDK_DEV_TOOLS)

#include <Windows.h>
#include <array>
#include <cstdio>
#include <format>
#include <string>

namespace dingosdk::game::usage_trace
{
    namespace
    {
        SRWLOCK file_lock = SRWLOCK_INIT;
        HANDLE trace_file = INVALID_HANDLE_VALUE;
        volatile LONG64 sequence{};

        struct FileLock
        {
            explicit FileLock(SRWLOCK &value) noexcept : lock(value)
            {
                AcquireSRWLockExclusive(&lock);
            }
            ~FileLock()
            {
                ReleaseSRWLockExclusive(&lock);
            }
            SRWLOCK &lock;
        };

        HANDLE open_file() noexcept
        {
            std::array<wchar_t, 32768> directory{};
            const auto length = GetEnvironmentVariableW(
                L"RESKATE_LOG_DIRECTORY", directory.data(), static_cast<DWORD>(directory.size()));
            if (!length || length >= directory.size())
                return INVALID_HANDLE_VALUE;
            std::wstring path(directory.data(), length);
            path += L"\\RvaUsage.jsonl";
            return CreateFileW(path.c_str(),
                               FILE_APPEND_DATA,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               nullptr,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);
        }

        void append_snapshot(std::string &line, const char *name, const StateSnapshot *snapshot)
        {
            line += ",\"";
            line += name;
            line += "\":";
            if (!snapshot || !snapshot->valid)
            {
                line += "null";
                return;
            }
            line += std::format("{{\"client_state\":{},\"game_type\":{},\"pending_requests\":{},"
                                "\"descriptions\":{}}}",
                                snapshot->client_state,
                                snapshot->game_type,
                                snapshot->pending_requests,
                                snapshot->descriptions);
        }
    }

    void record_hit(std::uintptr_t image_base,
                    std::uintptr_t rva,
                    std::string_view symbol,
                    std::span<const std::uint64_t> arguments) noexcept
    {
        static volatile LONG64 first{};
        static volatile LONG64 total_calls{};
        static volatile LONG64 interval_calls{};
        static volatile LONG64 next_report{};
        const auto now = GetTickCount64();
        const auto total = InterlockedIncrement64(&total_calls);
        const auto interval = InterlockedIncrement64(&interval_calls);
        if (InterlockedCompareExchange64(&first, 1, 0) == 0)
        {
            InterlockedExchange64(&next_report, static_cast<LONG64>(now + 5000));
            record_call("first", image_base, rva, symbol, arguments, nullptr, nullptr);
            return;
        }
        const auto due = InterlockedCompareExchange64(&next_report, 0, 0);
        if (now >= static_cast<ULONGLONG>(due) &&
            InterlockedCompareExchange64(&next_report, static_cast<LONG64>(now + 5000), due) == due)
        {
            const auto count = InterlockedExchange64(&interval_calls, 0);
            const std::array<std::uint64_t, 2> arguments{
                static_cast<std::uint64_t>(count), static_cast<std::uint64_t>(total)};
            record_call("summary", image_base, rva, symbol, arguments, nullptr, nullptr, 5000);
        }
        (void)interval;
    }

    void record_call(std::string_view phase,
                     std::uintptr_t image_base,
                     std::uintptr_t rva,
                     std::string_view symbol,
                     std::span<const std::uint64_t> arguments,
                     const StateSnapshot *before,
                     const StateSnapshot *after,
                     std::uint64_t elapsed_ms) noexcept
    {
        try
        {
            FileLock lock(file_lock);
            const auto event_sequence = InterlockedIncrement64(&sequence);
            std::string line = std::format(
                "{{\"sequence\":{},\"tick_ms\":{},\"thread_id\":{},\"phase\":\"{}\","
                "\"symbol\":\"{}\",\"rva\":\"0x{:X}\",\"address\":\"0x{:X}\",\"arguments\":[",
                event_sequence,
                GetTickCount64(),
                GetCurrentThreadId(),
                phase,
                symbol,
                rva,
                image_base + rva);
            for (std::size_t i = 0; i < arguments.size(); ++i)
            {
                if (i)
                    line += ',';
                line += std::format("\"0x{:X}\"", arguments[i]);
            }
            line += ']';
            append_snapshot(line, "before", before);
            append_snapshot(line, "after", after);
            line += std::format(",\"elapsed_ms\":{}}}\n", elapsed_ms);

            if (trace_file == INVALID_HANDLE_VALUE)
                trace_file = open_file();
            if (trace_file != INVALID_HANDLE_VALUE)
            {
                DWORD written{};
                (void)WriteFile(trace_file, line.data(), static_cast<DWORD>(line.size()), &written, nullptr);
            }
        }
        catch (...)
        {
        }
    }
}

#endif
