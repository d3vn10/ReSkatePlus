#include "Extension/Console/memory_ledger.h"
#include "Engine/Core/Hooks/hooks.h"
#include "Engine/Core/Log/logging.h"
#include "Engine/Core/Text/ascii.h"
#include <Windows.h>
#include <Psapi.h>
#include <dxgi1_4.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <format>
#include <fstream>
#include <map>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dingosdk::memory_ledger
{
    namespace
    {
        std::string megabytes(std::uint64_t bytes)
        {
            return bytes >= (1ull << 30) ? std::format("{:.2f} GB", static_cast<double>(bytes) / (1ull << 30))
                                         : std::format("{:.1f} MB", static_cast<double>(bytes) / (1ull << 20));
        }
		
        // ---- heaps ----
        struct SizeCount
        {
            std::uint64_t size{}, count{};
        };
        // Block sizes go into storage made before a heap is locked: allocating from a heap while
        // walking it would change what is walked.
        struct SizeTable
        {
            static constexpr std::size_t slots = std::size_t{1} << 16;
            std::vector<SizeCount> counts = std::vector<SizeCount>(slots);
            std::uint64_t other_count{}, other_bytes{};
            void add(std::uint64_t size) noexcept
            {
                for (std::size_t probe = 0; probe < 64; ++probe)
                {
                    auto &slot = counts[((size * 0x9e3779b97f4a7c15ull >> 48) + probe) & (slots - 1)];
                    if (slot.count && slot.size != size)
                        continue;
                    slot.size = size;
                    ++slot.count;
                    return;
                }
                ++other_count;
                other_bytes += size;
            }
        };
        struct HeapSeen
        {
            HANDLE heap{};
            std::uint64_t committed{}, busy{}, free{}, largest_free{}, busy_blocks{};
            bool whole{};
        };
        // (A fault in the walk is caught: a heap made without its own lock may change under it.)
        void walk_heap(HeapSeen &heap, SizeTable &sizes) noexcept
        {
            if (!HeapLock(heap.heap))
                return;
            heap.whole = true;
            __try
            {
                PROCESS_HEAP_ENTRY entry{};
                while (HeapWalk(heap.heap, &entry))
                {
                    if (entry.wFlags & PROCESS_HEAP_REGION)
                    {
                        heap.committed += entry.Region.dwCommittedSize;
                    }
                    else if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY)
                    {
                        heap.busy += entry.cbData;
                        ++heap.busy_blocks;
                        sizes.add(entry.cbData);
                    }
                    else if (!(entry.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE))
                    {
                        heap.free += entry.cbData;
                        heap.largest_free = std::max<std::uint64_t>(heap.largest_free, entry.cbData);
                    }
                }
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                heap.whole = false;
            }
            HeapUnlock(heap.heap);
        }
        std::mutex heap_mutex;
        std::map<std::uint64_t, std::uint64_t> heap_mark; // block size -> count
        bool heap_marked{};
    } // namespace

    void heap_report(const Line &out, bool mark, std::size_t most_sizes)
    {
        std::lock_guard report_lock(heap_mutex);
        struct Summary
        {
            DWORD size;
            SIZE_T allocated, committed, reserved, most;
        };
        static const auto heap_summary = reinterpret_cast<BOOL(WINAPI *)(HANDLE, DWORD, Summary *)>(
            GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "HeapSummary"));
        std::array<HANDLE, 256> heaps{};
        const auto heap_count = std::min<DWORD>(GetProcessHeaps(static_cast<DWORD>(heaps.size()), heaps.data()),
                                                static_cast<DWORD>(heaps.size()));
        SizeTable sizes;
        std::vector<HeapSeen> seen(heap_count);
        std::uint64_t total_committed{}, total_busy{}, total_free{};
        std::size_t partial{};
        for (DWORD i = 0; i < heap_count; ++i)
        {
            auto &heap = seen[i];
            heap.heap = heaps[i];
            walk_heap(heap, sizes);
            partial += !heap.whole;
            // (A heap whose segments the walk does not show, such as one of the segment heap's: its
            // summary says what it holds.)
            if (Summary summary{sizeof(Summary)};
                heap_summary && heap_summary(heaps[i], 0, &summary) && summary.committed > heap.committed)
                heap.committed = summary.committed;
            total_committed += heap.committed;
            total_busy += heap.busy;
            total_free += heap.free;
        }
        out(std::format("Heaps: {} committed in {} heap(s), {} of it in use, {} free inside them{}.",
                        megabytes(total_committed),
                        heap_count,
                        megabytes(total_busy),
                        megabytes(total_free),
                        partial ? std::format(" ({} could not be walked whole)", partial) : std::string()));
        std::ranges::sort(seen, [](const HeapSeen &a, const HeapSeen &b) { return a.committed > b.committed; });
        for (std::size_t i = 0; i < seen.size() && i < 8; ++i)
        {
            if (!seen[i].committed)
                continue;
            out(std::format("  heap {:#x}{}: {} committed, {} in use in {} blocks, {} free (the largest free piece {})",
                            reinterpret_cast<std::uintptr_t>(seen[i].heap),
                            seen[i].heap == GetProcessHeap() ? " (the process heap)" : "",
                            megabytes(seen[i].committed),
                            megabytes(seen[i].busy),
                            seen[i].busy_blocks,
                            megabytes(seen[i].free),
                            megabytes(seen[i].largest_free)));
        }
        std::map<std::uint64_t, std::uint64_t> now;
        for (const auto &slot : sizes.counts)
            if (slot.count)
                now[slot.size] = slot.count;
        std::vector<std::pair<std::uint64_t, std::uint64_t>> order(now.begin(), now.end()); // size, count
        std::ranges::sort(order, [](const auto &a, const auto &b) { return a.first * a.second > b.first * b.second; });
        out("Blocks in use by size, the sizes that add up to most first:");
        for (std::size_t i = 0; i < order.size() && i < most_sizes; ++i)
            out(std::format(
                "  {} x {} bytes = {}", order[i].second, order[i].first, megabytes(order[i].first * order[i].second)));
        if (sizes.other_count)
            out(std::format("  (and {} blocks of other sizes, {})", sizes.other_count, megabytes(sizes.other_bytes)));
        if (mark)
        {
            heap_mark = std::move(now);
            heap_marked = true;
            out("Marked. Do what you want to measure, then run `perf memory heap` again.");
            return;
        }
        if (!heap_marked)
            return;
        // What was added since the mark, by size: a leak shows as a size whose count only climbs.
        std::vector<std::tuple<std::uint64_t, std::int64_t>> added; // size, count added
        for (const auto &[size, count] : now)
        {
            const auto before = heap_mark.find(size);
            const auto was = before == heap_mark.end() ? 0 : before->second;
            if (count > was)
                added.emplace_back(size, static_cast<std::int64_t>(count - was));
        }
        std::ranges::sort(added,
                          [](const auto &a, const auto &b)
                          { return std::get<0>(a) * std::get<1>(a) > std::get<0>(b) * std::get<1>(b); });
        out("Added since the mark, by size:");
        for (std::size_t i = 0; i < added.size() && i < most_sizes; ++i)
            out(std::format("  +{} x {} bytes = {}",
                            std::get<1>(added[i]),
                            std::get<0>(added[i]),
                            megabytes(std::get<0>(added[i]) * static_cast<std::uint64_t>(std::get<1>(added[i])))));
    }

    namespace
    {
        struct Gpu
        {
            std::string name;
            std::uint64_t local{}, local_budget{}, shared{}, shared_budget{};
        };
        std::vector<Gpu> read_gpus()
        {
            std::vector<Gpu> gpus;
            IDXGIFactory4 *factory{};
            if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory4), reinterpret_cast<void **>(&factory))) || !factory)
                return gpus;
            IDXGIAdapter1 *adapter{};
            for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
            {
                DXGI_ADAPTER_DESC1 description{};
                IDXGIAdapter3 *adapter3{};
                if (SUCCEEDED(adapter->GetDesc1(&description)) && !(description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) &&
                    SUCCEEDED(adapter->QueryInterface(__uuidof(IDXGIAdapter3), reinterpret_cast<void **>(&adapter3))))
                {
                    DXGI_QUERY_VIDEO_MEMORY_INFO local{}, shared{};
                    adapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &local);
                    adapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &shared);
                    // (Only an adapter this process uses.)
                    if (local.CurrentUsage || shared.CurrentUsage)
                    {
                        Gpu gpu{{}, local.CurrentUsage, local.Budget, shared.CurrentUsage, shared.Budget};
                        for (const wchar_t *c = description.Description; *c; ++c)
                            gpu.name += static_cast<char>(*c < 128 ? *c : '?');
                        gpus.push_back(std::move(gpu));
                    }
                    adapter3->Release();
                }
                adapter->Release();
            }
            factory->Release();
            return gpus;
        }
    } // namespace

    void gpu_report(const Line &out)
    {
        for (const auto &gpu : read_gpus())
            out(std::format("GPU {}: {} of video memory (budget {}), {} of system memory (budget {}).",
                            gpu.name,
                            megabytes(gpu.local),
                            megabytes(gpu.local_budget),
                            megabytes(gpu.shared),
                            megabytes(gpu.shared_budget)));
    }

    std::string owners_line()
    {
        return {};
    }
    std::string gpu_line()
    {
        std::string line;
        for (const auto &gpu : read_gpus())
            line += std::format("{}GPU: {} video memory, {} system memory",
                                line.empty() ? "" : "; ",
                                megabytes(gpu.local),
                                megabytes(gpu.shared));
        return line;
    }

    std::filesystem::path write_report()
    {
        SYSTEMTIME now{};
        GetLocalTime(&now);
        const auto path = logging::status().directory / std::format("memory-{:04}{:02}{:02}-{:02}{:02}{:02}.txt",
                                                                    now.wYear,
                                                                    now.wMonth,
                                                                    now.wDay,
                                                                    now.wHour,
                                                                    now.wMinute,
                                                                    now.wSecond);
        std::string text;
        const Line add = [&](const std::string &line) { text += line + "\n"; };
        heap_report(add, false, 60);
        std::ofstream file(path, std::ios::binary);
        file << text;
        return file ? path : std::filesystem::path{};
    }
} // namespace dingosdk::memory_ledger
