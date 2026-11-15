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
        volatile LONG64 dropped_events{};

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
            const std::wstring directory_path(directory.data(), length);
            for (std::uint64_t index = 1; index != 0; ++index)
            {
                const auto path = directory_path + L"\\RvaUsage" + std::to_wstring(index) + L".json";
                const auto file = CreateFileW(path.c_str(),
                                              FILE_APPEND_DATA,
                                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                              nullptr,
                                              CREATE_NEW,
                                              FILE_ATTRIBUTE_NORMAL,
                                              nullptr);
                if (file != INVALID_HANDLE_VALUE)
                    return file;

                const auto error = GetLastError();
                if (error != ERROR_FILE_EXISTS && error != ERROR_ALREADY_EXISTS)
                    return INVALID_HANDLE_VALUE;
            }
            return INVALID_HANDLE_VALUE;
        }

        void append_json_string(std::string &out, std::string_view value)
        {
            static constexpr char hex[] = "0123456789ABCDEF";
            out.push_back('"');
            for (const unsigned char ch : value)
            {
                switch (ch)
                {
                case '"': out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\b': out += "\\b"; break;
                case '\f': out += "\\f"; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                default:
                    if (ch < 0x20)
                    {
                        out += "\\u00";
                        out.push_back(hex[ch >> 4]);
                        out.push_back(hex[ch & 0x0f]);
                    }
                    else
                        out.push_back(static_cast<char>(ch));
                }
            }
            out.push_back('"');
        }

        void write_line(std::string &line) noexcept
        {
            if (trace_file == INVALID_HANDLE_VALUE)
                trace_file = open_file();
            if (trace_file == INVALID_HANDLE_VALUE)
            {
                InterlockedIncrement64(&dropped_events);
                return;
            }
            std::size_t offset{};
            while (offset < line.size())
            {
                DWORD written{};
                const auto remaining = line.size() - offset;
                const auto chunk = static_cast<DWORD>(std::min<std::size_t>(remaining, MAXDWORD));
                if (!WriteFile(trace_file, line.data() + offset, chunk, &written, nullptr) || !written)
                {
                    CloseHandle(trace_file);
                    trace_file = INVALID_HANDLE_VALUE;
                    InterlockedIncrement64(&dropped_events);
                    return;
                }
                offset += written;
            }
        }

        void append_dropped(std::string &line)
        {
            const auto dropped = InterlockedExchange64(&dropped_events, 0);
            line += std::format(",\\"dropped_before\\":{}", dropped);
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
            const std::array<std::uint64_t, 2> summary_arguments{
                static_cast<std::uint64_t>(count), static_cast<std::uint64_t>(total)};
            record_call("summary", image_base, rva, symbol, summary_arguments, nullptr, nullptr, 5000);
        }
        (void)interval;
    }

    void record_lifecycle(std::string_view subsystem,
                          std::string_view action,
                          std::string_view name,
                          std::uintptr_t address,
                          std::uintptr_t related_address,
                          std::size_t size,
                          std::int64_t status) noexcept
    {
        try
        {
            FileLock lock(file_lock);
            const auto event_sequence = InterlockedIncrement64(&sequence);
            std::string line = std::format(
                "{{\\"sequence\\":{},\\"tick_ms\\":{},\\"thread_id\\":{},\\"type\\":\\"lifecycle\\",\\"subsystem\\":",
                event_sequence, GetTickCount64(), GetCurrentThreadId());
            append_json_string(line, subsystem);
            line += ",\\"action\\":";
            append_json_string(line, action);
            line += ",\\"name\\":";
            append_json_string(line, name);
            line += std::format(
                ",\\"address\\":\\"0x{:X}\\",\\"related_address\\":\\"0x{:X}\\","
                "\\"size\\":{},\\"status\\":{}",
                address, related_address, size, status);
            append_dropped(line);
            line += "}\\n";
            write_line(line);
        }
        catch (...)
        {
            InterlockedIncrement64(&dropped_events);
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
            FileLock lock(file_lock);
            const auto event_sequence = InterlockedIncrement64(&sequence);
            std::string line = std::format(
                "{{\\"sequence\\":{},\\"tick_ms\\":{},\\"thread_id\\":{},\\"phase\\":",
                event_sequence,
                GetTickCount64(),
                GetCurrentThreadId());
            append_json_string(line, phase);
            line += ",\\"symbol\\":";
            append_json_string(line, symbol);
            line += std::format(",\\"rva\\":\\"0x{:X}\\",\\"address\\":\\"0x{:X}\\",\\"arguments\\":[",
                                rva, image_base + rva);
            for (std::size_t i = 0; i < arguments.size(); ++i)
            {
                if (i)
                    line += ',';
                line += std::format("\"0x{:X}\"", arguments[i]);
            }
            line += ']';
            append_snapshot(line, "before", before);
            append_snapshot(line, "after", after);
            line += std::format(",\\"elapsed_ms\\":{}", elapsed_ms);
            append_dropped(line);
            line += "}\\n";
            write_line(line);
        }
        catch (...)
        {
        }
    }
}

#endif
