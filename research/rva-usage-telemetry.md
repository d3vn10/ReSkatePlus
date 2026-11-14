# RVA usage telemetry (ReSkatePlus 2.2.0 research branch)

This branch carries a development-only JSONL trace on top of the current ReSkatePlus `main` codebase. It does not change the accepted Skate executable or data build. The tracer writes `RvaUsage.jsonl` separately from `ReSkate.log`, using the launcher's `RESKATE_LOG_DIRECTORY` environment variable.

## Instrumented calls

| Symbol | RVA in the currently pinned 20260929 address set | Events |
|---|---:|---|
| `client_tick` | `0x2E77190` | First hit, then five-second call-count/total summaries |
| `client_state_transition` | `0x2E6BF40` | Observer armed, enter, exit; client-state snapshot |
| `client_state_request` | `0x2E722B0` | Observer armed, enter, exit; client-state snapshot |
| `connection_disconnect` | `0x2E9F090` | Observer armed, enter, exit; connection and reason arguments |

These RVAs are read from the pinned `Engine/Game/Build/20260929/` headers; they are not proposed replacements for another Skate build.

## Event fields and interpretation

Each JSONL event includes a monotonic sequence number, process tick, thread ID, phase, symbol, RVA, absolute address, scalar/pointer argument values, optional before/after snapshots, and elapsed time. The three lifecycle functions are void functions, so the tracer records that they returned and the elapsed time, not a return value.

The two client-state functions sample client state, game type, pending-request count, and description count. A snapshot is null if the guarded reads cannot all be completed. Disconnect records its connection pointer and reason but does not dereference the connection.

The tick is deliberately aggregated rather than writing every frame. A missing event means only that this instrumented call site was not observed in that session; it does not prove that an uninstrumented function, global, vtable, layout, or patch site was unused.

## Safety and overhead

- Compiled only when `DINGOSDK_DEV_TOOLS` is enabled; release builds do not include the tracer.
- Pointer arguments are logged as addresses only. The tracer does not dump pointed-to memory, network payloads, credentials, or user files.
- The trace file is created/truncated on the first telemetry event and appended under a process-local lock.
- Hook installation still requires the existing function fingerprints. The tracer does not weaken or bypass those guards.
- The launcher already sets `RESKATE_LOG_DIRECTORY` for the child process.
- This is a pilot covering four function entry points, not a census of every ReSkate dependency.

## Suggested capture session

Use a development build against the currently supported game only. Start the game, remain at the main menu, enter a level, perform ordinary movement/gameplay, change levels, return to the menu, and exit normally. Preserve the matching `ReSkate.log`, `RvaUsage.jsonl`, exact Skate.exe hash, and Data changelist together. Compare separate sessions by scenario; absence of a trace event is not evidence about uninstrumented dependencies.
