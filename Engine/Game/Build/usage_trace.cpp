#include "usage_trace.h"

#if defined(DINGOSDK_DEV_TOOLS)

#include <Windows.h>
#include <array>
#include <cstdio>
#include <string>

namespace dingosdk::game::usage_trace
{
    namespace
    {
        SRWLOCK file_lock = SRWLOCK_INIT;
        HANDLE trace_file = INVALID_HANDLE_VALUE;
        volatile LONG64 sequence{};

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
                               OPEN_ALWAYS,
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
            std::string line = std::format(
                "{{\"sequence\":{},\"tick_ms\":{},\"thread_id\":{},\"phase\":\"{}\","
                "\"symbol\":\"{}\",\"rva\":\"0x{:X}\",\"address\":\"0x{:X}\",\"arguments\":[",
                InterlockedIncrement64(&sequence),
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

            AcquireSRWLockExclusive(&file_lock);
            if (trace_file == INVALID_HANDLE_VALUE)
                trace_file = open_file();
            if (trace_file != INVALID_HANDLE_VALUE)
            {
                DWORD written{};
                (void)WriteFile(trace_file, line.data(), static_cast<DWORD>(line.size()), &written, nullptr);
            }
            ReleaseSRWLockExclusive(&file_lock);
        }
        catch (...)
        {
        }
    }
}

#endif
