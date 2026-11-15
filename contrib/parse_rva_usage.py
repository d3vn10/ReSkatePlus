#!/usr/bin/env python3
"""Print a summary of ReSkatePlus RvaUsage JSONL telemetry."""

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


def summarize(events: list[dict[str, Any]]) -> tuple[int, int, int, Counter[str], Counter[str], list[dict[str, Any]]]:
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

    type_counts: Counter[str] = Counter()
    category_counts: Counter[str] = Counter()
    by_symbol: OrderedDict[str, dict[str, Any]] = OrderedDict()
    events_by_rva: Counter[str] = Counter()

    for event in events:
        event_type = str(event.get("type") or "unspecified")
        type_counts[event_type] += 1

        # Cover both lifecycle records (type/subsystem/action) and function
        # instrumentation records (symbol/phase), without assuming one schema.
        if event.get("action") is not None:
            category = "/".join(
                str(value)
                for value in (event.get("subsystem"), event.get("action"))
                if value is not None
            )
        elif event.get("symbol") is not None:
            category = f"symbol/{event.get('symbol')}/{event.get('phase', 'event')}"
        else:
            category = event_type
        category_counts[category] += 1

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
        by_symbol[symbol]["symbol_events"] += 1

    rows = []
    for row in by_symbol.values():
        rva = row["rva"]
        rows.append({
            "symbol": row["symbol"],
            "rva": rva,
            "address": row["address"],
            # Observer and target records can share an RVA. Keep observer rows
            # separate; target counts include every symbol event at that RVA.
            "calls": (
                row["symbol_events"]
                if row["symbol"].endswith("_observer")
                else events_by_rva[rva]
            ),
        })

    return len(events), max_tick, primary_thread, type_counts, category_counts, rows


def format_report(path: Path, events: list[dict[str, Any]]) -> str:
    sequences, ticks, thread, type_counts, category_counts, rows = summarize(events)

    output = [
        f"FILE\t\t{path.name}",
        f"EVENTS\t\t{sequences}",
        f"MAX TICK\t{ticks}",
        f"PRIMARY THREAD\t{thread}",
        "",
        "EVENT COUNTS BY TYPE",
    ]
    for event_type, count in type_counts.most_common():
        output.append(f"{event_type:<20}\t{count}")

    output.extend(["", "EVENT COUNTS BY SUBSYSTEM/ACTION OR SYMBOL/PHASE"])
    for category, count in category_counts.most_common():
        output.append(f"{category:<60}\t{count}")

    if rows:
        symbol_width = max(6, *(len(row["symbol"]) for row in rows))
        rva_width = max(3, *(len(row["rva"]) for row in rows))
        address_width = max(7, *(len(row["address"]) for row in rows))
        output.extend([
            "",
            "FUNCTION SYMBOL SUMMARY",
            f"{'SYMBOL':<{symbol_width}}\t{'RVA':<{rva_width}}\t"
            f"{'ADDRESS':<{address_width}}\tEVENTS",
        ])
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
        description="Print an event summary and function-symbol table from an RvaUsage JSONL file."
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
