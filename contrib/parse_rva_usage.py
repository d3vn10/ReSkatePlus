#!/usr/bin/env python3
"""Summarize ReSkatePlus RvaUsage JSONL telemetry files."""

from __future__ import annotations

import argparse
import json
import sys
from collections import Counter, OrderedDict
from pathlib import Path
from typing import Any


def load_events(path: Path) -> list[dict[str, Any]]:
    events: list[dict[str, Any]] = []
    try:
        with path.open("r", encoding="utf-8-sig") as stream:
            for line_number, line in enumerate(stream, 1):
                if not line.strip():
                    continue
                try:
                    event = json.loads(line)
                except json.JSONDecodeError as exc:
                    raise ValueError(
                        f"{path}:{line_number}: invalid JSON: {exc.msg}"
                    ) from exc
                if not isinstance(event, dict):
                    raise ValueError(
                        f"{path}:{line_number}: expected a JSON object"
                    )
                events.append(event)
    except OSError as exc:
        raise ValueError(f"cannot read {path}: {exc}") from exc

    if not events:
        raise ValueError(f"{path}: no telemetry events found")
    return events


def summarize(events: list[dict[str, Any]]) -> tuple[int, int, int, list[dict[str, Any]]]:
    thread_counts = Counter(
        str(event["thread_id"])
        for event in events
        if event.get("thread_id") is not None
    )
    # The most frequently observed thread is the primary telemetry thread.
    primary_thread = int(thread_counts.most_common(1)[0][0]) if thread_counts else 0

    ticks = [
        int(event["tick_ms"])
        for event in events
        if isinstance(event.get("tick_ms"), (int, float))
    ]
    max_tick = max(ticks, default=0)

    by_symbol: OrderedDict[str, dict[str, Any]] = OrderedDict()
    events_by_rva: Counter[str] = Counter()

    for event in events:
        symbol = event.get("symbol")
        if not isinstance(symbol, str) or not symbol:
            continue
        rva = str(event.get("rva", ""))
        address = str(event.get("address", ""))
        events_by_rva[rva] += 1

        if symbol not in by_symbol:
            by_symbol[symbol] = {
                "symbol": symbol,
                "rva": rva,
                "address": address,
                "symbol_events": 0,
            }
        row = by_symbol[symbol]
        row["symbol_events"] += 1

        # A periodic client_tick summary stores the cumulative invocation count
        # in arguments[1]. The table intentionally reports telemetry events,
        # not inferred invocations, so this value is not substituted here.

    rows = []
    for row in by_symbol.values():
        rva = row["rva"]
        rows.append({
            "symbol": row["symbol"],
            "rva": rva,
            "address": row["address"],
            # The instrumented observer and target can share an RVA. Count all
            # events at that address in the target's row; keep observer rows too.
            "calls": (
                row["symbol_events"]
                if row["symbol"].endswith("_observer")
                else events_by_rva[rva]
            ),
            "symbol_events": row["symbol_events"],
        })

    return len(events), max_tick, primary_thread, rows


def format_report(path: Path, events: list[dict[str, Any]]) -> str:
    sequences, ticks, thread, rows = summarize(events)

    symbol_width = max(6, *(len(row["symbol"]) for row in rows))
    rva_width = max(3, *(len(row["rva"]) for row in rows))
    address_width = max(7, *(len(row["address"]) for row in rows))

    output = [
        f"FILE\t\t{path.name}",
        f"SEQUENCES\t{sequences}",
        f"TICKS\t\t{ticks}",
        f"THREAD\t\t{thread}",
        "",
        f"{'SYMBOL':<{symbol_width}}\t{'RVA':<{rva_width}}\t{'ADDRESS':<{address_width}}\tCALLS",
    ]
    for row in rows:
        output.append(
            f"{row['symbol']:<{symbol_width}}\t"
            f"{row['rva']:<{rva_width}}\t"
            f"{row['address']:<{address_width}}\t"
            f"{row['calls']}"
        )
    return "\n".join(output)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Print a per-symbol summary of an RvaUsage JSONL file."
    )
    parser.add_argument("file", type=Path, help="Path to RvaUsageN.json")
    args = parser.parse_args()

    try:
        events = load_events(args.file)
        print(format_report(args.file, events))
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
