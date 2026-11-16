#!/usr/bin/env python3
"""Generate a build-specific ReSkatePlus header directory from an RVA sweep.

The 20260929 headers are copied as the structural baseline. Numeric RVA
constants in companion *_rvas.h files are replaced with accepted 20261013
candidates; unresolved, low-scoring, or conflicting entries become 0x0.

A candidate is accepted when its reported similarity is >= --minimum-score,
or its discovery status is one of the exact-match statuses. An optional AOB
fallback report is processed by the same rule.
"""
from __future__ import annotations

import argparse
import json
import re
import shutil
import sys
from collections import defaultdict
from pathlib import Path, PurePosixPath
from typing import Any

EXACT_STATUSES = {
    "EXACT_SIGNATURE_UNIQUE",
    "EXACT_FUNCTION_BYTES_UNIQUE",
    "EXACT_BEHAVIORAL_REFERENCE_CODE_UNIQUE",
    "EXACT_BEHAVIORAL_REFERENCE_BYTES_UNIQUE",
}
RVA_DECLARATION = re.compile(
    r"(?P<prefix>\b(?:inline\s+)?constexpr\s+"
    r"(?:std::)?(?:uintptr_t|uint32_t|uint64_t|RVA|Address)\s+"
    r"(?P<symbol>[A-Za-z_]\w*)\s*=\s*)"
    r"(?P<value>0[xX][0-9a-fA-F]+)(?P<suffix>\s*;)"
)


def load_report(path: Path) -> dict[str, Any]:
    try:
        report = json.loads(path.read_text(encoding="utf-8-sig"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ValueError(f"cannot read report {path}: {exc}") from exc
    if not isinstance(report, dict) or not isinstance(report.get("results"), list):
        raise ValueError(f"{path} is not a sweep report with a results array")
    return report


def accepted_candidate(record: dict[str, Any], minimum_score: float) -> int | None:
    candidate = record.get("candidate_rva")
    if not isinstance(candidate, int) or candidate <= 0:
        return None
    status = str(record.get("discovery_status", record.get("status", "")))
    score = record.get("similarity_percent")
    if status in EXACT_STATUSES:
        return candidate
    if isinstance(score, (int, float)) and float(score) >= minimum_score:
        return candidate
    return None


def build_mapping(
    reports: list[dict[str, Any]], minimum_score: float
) -> tuple[dict[tuple[str, str], int], set[tuple[str, str]], int]:
    candidates: dict[tuple[str, str], set[int]] = defaultdict(set)
    seen: set[tuple[str, str]] = set()
    accepted_records = 0
    for report in reports:
        for record in report["results"]:
            if not isinstance(record, dict):
                continue
            header, symbol = record.get("header"), record.get("symbol")
            if not isinstance(header, str) or not isinstance(symbol, str):
                continue
            key = (PurePosixPath(header).name, symbol)
            seen.add(key)
            candidate = accepted_candidate(record, minimum_score)
            if candidate is not None:
                candidates[key].add(candidate)
                accepted_records += 1

    mapping: dict[tuple[str, str], int] = {}
    conflicts: set[tuple[str, str]] = set()
    for key in seen:
        values = candidates.get(key, set())
        if len(values) == 1:
            mapping[key] = next(iter(values))
        elif len(values) > 1:
            conflicts.add(key)
            mapping[key] = 0
        else:
            mapping[key] = 0
    return mapping, conflicts, accepted_records


def generate(
    source_dir: Path,
    output_dir: Path,
    mapping: dict[tuple[str, str], int],
    candidate_hash: str | None,
    force: bool,
) -> dict[str, int]:
    if not source_dir.is_dir():
        raise ValueError(f"source header directory does not exist: {source_dir}")
    if output_dir.exists():
        if not force:
            raise ValueError(
                f"output directory already exists: {output_dir}; pass --force to replace it"
            )
        shutil.rmtree(output_dir)
    output_dir.mkdir(parents=True)

    stats = {"headers": 0, "rva_constants": 0, "mapped": 0, "missing": 0, "unreported": 0}
    for source in sorted(source_dir.rglob("*.h")):
        relative = source.relative_to(source_dir)
        target = output_dir / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        text = source.read_text(encoding="utf-8-sig", errors="replace")
        text = text.replace("v20260929", "v20261013")
        text = text.replace("September 29 2026 executable RVAs", "October 13 2026 executable RVAs")
        text = text.replace("September 29 2026", "October 13 2026")
        if candidate_hash:
            text = text.replace(
                "fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9",
                candidate_hash,
            )

        if source.stem.endswith("_rvas"):
            owner_header = source.stem[:-5] + ".h"

            def replace(match: re.Match[str]) -> str:
                value = int(match.group("value"), 16)
                # Small constants are offsets/flags/layout values, not image RVAs.
                if value < 0x10000 or value >= 0x80000000:
                    return match.group(0)
                stats["rva_constants"] += 1
                key = (owner_header, match.group("symbol"))
                if key not in mapping:
                    stats["unreported"] += 1
                    rendered = "0x0"
                elif mapping[key] == 0:
                    stats["missing"] += 1
                    rendered = "0x0"
                else:
                    stats["mapped"] += 1
                    rendered = f"0x{mapping[key]:x}"
                return match.group("prefix") + rendered + match.group("suffix")

            text = RVA_DECLARATION.sub(replace, text)

        target.write_text(text, encoding="utf-8", newline="\n")
        stats["headers"] += 1
    return stats


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-dir", type=Path, required=True,
                        help="ReSkatePlus Engine/Game/Build/20260929 directory")
    parser.add_argument("--report", type=Path, required=True,
                        help="rva-sweep-results.json")
    parser.add_argument("--aob-report", type=Path,
                        help="Optional rva-sweep-missing-aob.json fallback report")
    parser.add_argument("--output-dir", type=Path,
                        help="Output directory (default: sibling 20261013)")
    parser.add_argument("--minimum-score", type=float, default=99.0,
                        help="Accept non-exact candidates at or above this score (default: 99)")
    parser.add_argument("--force", action="store_true",
                        help="Replace the output directory if it already exists")
    args = parser.parse_args()

    if not 0.0 <= args.minimum_score <= 100.0:
        parser.error("--minimum-score must be between 0 and 100")
    output_dir = args.output_dir or args.source_dir.parent / "20261013"
    try:
        reports = [load_report(args.report)]
        if args.aob_report:
            reports.append(load_report(args.aob_report))
        mapping, conflicts, accepted_records = build_mapping(reports, args.minimum_score)
        candidate_hash = reports[0].get("candidate", {}).get("sha256")
        if not isinstance(candidate_hash, str):
            candidate_hash = reports[0].get("candidate_sha256")
        stats = generate(args.source_dir, output_dir, mapping, candidate_hash, args.force)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    print(f"Generated headers: {output_dir}")
    print(f"Headers copied: {stats['headers']}")
    print(f"RVA constants: {stats['rva_constants']}")
    print(f"Mapped RVAs: {stats['mapped']}")
    print(f"Reported but unresolved: {stats['missing']}")
    print(f"Not present in either report: {stats['unreported']}")
    print(f"Conflicting assignments set to 0x0: {len(conflicts)}")
    print(f"Accepted report records: {accepted_records}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
