# RVA usage telemetry (development branch)

This branch adds an opt-in, development-only JSONL trace for selected native calls. It is deliberately separate from `ReSkate.log` and creates a fresh `RvaUsage.jsonl` for each launch in the launcher's `RESKATE_LOG_DIRECTORY`.

## Event meaning

Detailed lifecycle hooks produce `enter` and `exit` records with a monotonically increasing sequence, process tick, thread ID, symbol, RVA, absolute address, and scalar/pointer argument values. `armed` records show which observers installed successfully; they are setup markers, not calls. Where a call has a useful state contract, the record includes explicit before/after snapshots and elapsed time. These initial targets are void functions, so there is no return value to record.

For a void function, an exit record and state snapshot are still useful. They establish that the native call returned and show selected observed state before and after; they do not prove that every asynchronous or indirect effect was caused by that call.

Pointer arguments are logged as addresses only. The tracer does not dereference arbitrary pointers, dump buffers, or collect user files, credentials, network payloads, or unrelated process memory.

## Scope and limitations

This is not yet a transparent recorder for every address in the build headers. ReSkate uses direct calls, hooks, globals, vtables, data layouts, and patch sites; a universal ABI-blind detour would be unsafe and would not know how to decode arguments. The first integration instruments the hot native client tick (`client_tick`) with a first-hit record and five-second call-count summaries, plus the existing level-transition, level-request, and disconnect hooks (`client_state_transition`, `client_state_request`, and `connection_disconnect`). The tick is intentionally aggregated rather than writing two records per frame, to keep telemetry overhead and file growth bounded. It records client state, game type, pending-request count, and description count around the first two; disconnect records its connection pointer and reason. The log file is created/truncated on the first telemetry event (a client-tick first-hit or an observer `armed` marker), so the file identifies that instrumentation was reached even if no detailed lifecycle call follows. Further call sites must be added with their actual ABI and useful state contracts.

A missing event means only that the instrumented call site was not observed in that session. It does not establish that an uninstrumented RVA or data dependency was unused.

The feature is compiled only with `DINGOSDK_DEV_TOOLS`; release builds do not include it. The normal supported-build checks remain mandatory. This branch also tightens them: the runtime independently checks the executable SHA-256, and unreadable/unknown game-data changelists fail closed.
