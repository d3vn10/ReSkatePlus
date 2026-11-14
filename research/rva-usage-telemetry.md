# RVA usage telemetry (development branch)

This branch adds an opt-in, development-only JSONL trace for selected native calls. It is deliberately separate from `ReSkate.log` and creates a fresh `RvaUsage.jsonl` for each launch in the launcher's `RESKATE_LOG_DIRECTORY`.

## Event meaning

Each call produces an `enter` and `exit` record with a monotonically increasing sequence, process tick, thread ID, symbol, RVA, absolute address, and scalar argument values. Where a call has a useful state contract, the record can include explicit before/after snapshots and elapsed time.

For a void function, an exit record and state snapshot are still useful. They establish that the native call returned and show selected observed state before and after; they do not prove that every asynchronous or indirect effect was caused by that call.

Pointer arguments are logged as addresses only. The tracer does not dereference arbitrary pointers, dump buffers, or collect user files, credentials, network payloads, or unrelated process memory.

## Scope and limitations

This is not yet a transparent recorder for every address in the build headers. ReSkate uses direct calls, hooks, globals, vtables, data layouts, and patch sites; a universal ABI-blind detour would be unsafe and would not know how to decode arguments. The first integration instruments the existing level-transition, level-request, and disconnect hooks. Further call sites must be added with their actual ABI and useful state contracts.

A missing event means only that the instrumented call site was not observed in that session. It does not establish that an uninstrumented RVA or data dependency was unused.

The feature is compiled only with `DINGOSDK_DEV_TOOLS`; release builds do not include it. The normal supported-build checks remain mandatory and unchanged.
