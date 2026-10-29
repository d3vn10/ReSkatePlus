#!/usr/bin/env python3
"""
Prepare a clean upstream ReSkate checkout, apply upstream.toml, then run build.ps1.

Requires Python 3.11+ (for tomllib) and Windows PowerShell/CMake/Visual Studio build tools.
Place replacement payload files in ./patch-files using the filenames in upstream.toml.
"""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import sys
import urllib.error
import urllib.request
import zipfile
from datetime import datetime
from pathlib import Path, PurePosixPath

try:
    import tomllib
except ImportError:
    print("ERROR: Python 3.11 or newer is required (the script uses tomllib).")
    raise SystemExit(1)

SCRIPT_DIR = Path(__file__).resolve().parent
TOML_PATH = SCRIPT_DIR / "upstream.toml"
BUILD_SCRIPT = SCRIPT_DIR / "build.ps1"
PATCH_FILES = SCRIPT_DIR / "patch-files"
WORK_DIR = SCRIPT_DIR / "build-work"

UPSTREAM_REPO = "Dingo-Shenanigans/ReSkate"

class PatchError(RuntimeError):
    pass

def github_json(url: str):
    print(f"[GitHub API] Requesting: {url}", flush=True)
    try:
        req = urllib.request.Request(
            url,
            headers={
                "Accept": "application/vnd.github+json",
                "User-Agent": "ReSkatePlus-Upstream-Builder",
                "X-GitHub-Api-Version": "2022-11-28",
            },
        )
        with urllib.request.urlopen(req, timeout=30) as response:
            return json.load(response)
    except Exception as exc:
        print(f"[GitHub API] Failed URL: {url}", flush=True)
        print(f"[GitHub API] Error: {exc}", flush=True)
        raise

def get_latest_commit(repo: str) -> dict:
    repo_info = github_json(f"https://api.github.com/repos/{repo}")
    branch = repo_info["default_branch"]
    commits = github_json(
        f"https://api.github.com/repos/{repo}/commits"
        f"?sha={branch}&per_page=1"
    )
    if not commits:
        raise RuntimeError(f"GitHub returned no commits for {repo}:{branch}.")
    commit = commits[0]
    return {
        "repo": repo,
        "branch": branch,
        "sha": commit["sha"],
        "date": commit["commit"]["committer"]["date"],
        "message": commit["commit"]["message"].splitlines(),
        "html_url": commit["html_url"],
    }

def get_upstream_latest_version(repo: str) -> str:
    """Queries the upstream GitHub releases endpoint to grab the clean base version string."""
    try:
        release_data = github_json(f"https://api.github.com/repos/{repo}/releases/latest")
        tag_name = release_data.get("tag_name", "2.1.3")
        return tag_name.lstrip("vV").strip()
    except Exception as exc:
        print(
            f"Warning: Could not fetch latest upstream release tag from API "
            f"({exc}). Defaulting to 2.1.3"
        )
        return "2.1.3"

def load_toml() -> dict:
    if not TOML_PATH.is_file():
        raise RuntimeError(f"Missing {TOML_PATH.name} next to this script.")
    try:
        with TOML_PATH.open("rb") as stream:
            return tomllib.load(stream)
    except tomllib.TOMLDecodeError as exc:
        raise RuntimeError(
            f"{TOML_PATH.name} is not valid TOML: {exc}\n"
            "Each patch must have its own [[patches]] table."
        ) from exc

def substitute(value: str, variables: dict, context: str) -> str:
    def replace(match: re.Match) -> str:
        key = match.group(1)
        if key not in variables:
            raise PatchError(f"{context}: unknown template variable {{{{{key}}}}}.")
        return str(variables[key])
    return re.sub(r"\{\{([A-Za-z_][A-Za-z0-9_]*)\}\}", replace, value)

def safe_repo_path(root: Path, relative: str, context: str) -> Path:
    rel = PurePosixPath(relative)
    if rel.is_absolute() or ".." in rel.parts:
        raise PatchError(f"{context}: unsafe repository path: {relative!r}")
    result = root.joinpath(*rel.parts)
    if not result.resolve().is_relative_to(root.resolve()):
        raise PatchError(f"{context}: path escapes the source directory: {relative!r}")
    return result

def apply_patches(source_root: Path, config: dict) -> int:
    variables = config.get("vars", {})
    patches = config.get("patches", [])
    if not isinstance(patches, list):
        raise PatchError("The TOML 'patches' value must be an array of tables.")
    staged_text: dict[Path, str] = {}
    staged_copies: list[tuple[Path, Path, int]] = []
    copy_sources: list[Path] = []
    patch_count = 0
    for index, patch in enumerate(patches, start=1):
        if "pattern" in patch:
            item = patch["pattern"]
            file_name = item.get("file")
            operation = item.get("operation")
            search = item.get("search")
            replacement = item.get("text", "")
            context = f"patch #{index} ({file_name})"
            if not all(
                isinstance(value, str) and value
                for value in (file_name, operation, search)
            ):
                raise PatchError(f"{context}: 'file', 'operation', and 'search' are required.")
            if operation not in {"at", "before", "after"}:
                raise PatchError(
                    f"{context}: unsupported operation {operation!r}; "
                    "use at, before, or after."
                )
            if not isinstance(replacement, str):
                raise PatchError(f"{context}: 'text' must be a string.")
            search = substitute(search, variables, context)
            replacement = substitute(replacement, variables, context)
            target = safe_repo_path(source_root, file_name, context)
            if target not in staged_text:
                if not target.is_file():
                    raise PatchError(f"{context}: target file does not exist.")
                try:
                    staged_text[target] = target.read_text(encoding="utf-8")
                except UnicodeDecodeError as exc:
                    raise PatchError(
                        f"{context}: {file_name} is not UTF-8 text; "
                        "use a replace/copy patch."
                    ) from exc
            original = staged_text[target]
            matches = original.count(search)
            expected = item.get("count", 1)
            if not isinstance(expected, int) or expected < 1:
                raise PatchError(f"{context}: 'count' must be a positive integer.")
            if matches != expected:
                raise PatchError(
                    f"{context}: unable to find expected text safely; "
                    f"expected {expected} match(es), found {matches}.\n"
                    f"Search text: {search[:240]!r}"
                )
            if operation == "before":
                staged_text[target] = original.replace(search, replacement + search)
            elif operation == "after":
                staged_text[target] = original.replace(search, search + replacement)
            else:
                staged_text[target] = original.replace(search, replacement)
            patch_count += 1
        elif "replace" in patch:
            item = patch["replace"]
            file_name = item.get("file")
            source_name = item.get("source")
            context = f"patch #{index} ({file_name})"
            if not isinstance(file_name, str) or not isinstance(source_name, str):
                raise PatchError(f"{context}: 'file' and 'source' are required.")
            source = safe_repo_path(PATCH_FILES, source_name, context)
            target = safe_repo_path(source_root, file_name, context)
            if not source.is_file():
                copy_sources.append(source)
            else:
                staged_copies.append((source, target, index))
            patch_count += 1
        else:
            raise PatchError(
                f"patch #{index}: expected a [patches.pattern] "
                "or [patches.replace] table."
            )
    if copy_sources:
        names = "\n".join(f"  - {path.name}" for path in copy_sources)
        raise PatchError(
            "Replacement payload file(s) are missing from the patch-files folder:\n"
            f"{names}\n"
            f"Create {PATCH_FILES} and put those files there."
        )
    for target, content in staged_text.items():
        target.write_text(content, encoding="utf-8", newline="")
    for source, target, _ in staged_copies:
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
    return patch_count

def download_checkout(commit: dict) -> Path:
    short_sha = commit["sha"][:12]
    checkout_dir = WORK_DIR / f"ReSkate-{short_sha}"
    if checkout_dir.exists():
        shutil.rmtree(checkout_dir)
    checkout_dir.mkdir(parents=True, exist_ok=True)
    archive_url = f"https://github.com/{commit['repo']}/archive/{commit['sha']}.zip"
    archive_path = checkout_dir / "upstream.zip"
    request = urllib.request.Request(
        archive_url,
        headers={"User-Agent": "ReSkatePlus-Upstream-Builder"},
    )
    print(f"Downloading {commit['repo']} at {short_sha}...")
    with urllib.request.urlopen(request, timeout=120) as response:
        with archive_path.open("wb") as output:
            shutil.copyfileobj(response, output)
    extracted = checkout_dir / "extracted"
    extracted.mkdir()
    with zipfile.ZipFile(archive_path) as archive:
        root = extracted.resolve()
        for member in archive.infolist():
            destination = (extracted / member.filename).resolve()
            if not destination.is_relative_to(root):
                raise RuntimeError("The downloaded source archive contains an unsafe path.")
        archive.extractall(extracted)
    archive_path.unlink(missing_ok=True)
    roots = [path for path in extracted.iterdir() if path.is_dir()]
    if len(roots) != 1:
        raise RuntimeError("Could not identify the source folder in the GitHub archive.")
    source_root = roots[0]
    final_root = checkout_dir / "source"
    source_root.rename(final_root)
    extracted.rmdir()
    return final_root

def run_build(source_root: Path, version: str) -> None:
    if not BUILD_SCRIPT.is_file():
        raise RuntimeError(f"Missing build script: {BUILD_SCRIPT}")
    powershell = shutil.which("pwsh") or shutil.which("powershell")
    if not powershell:
        raise RuntimeError("Could not find PowerShell (pwsh or powershell) on PATH.")
    print(f"\nRunning build.ps1 for version {version}...")
    result = subprocess.run(
        [
            powershell,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(BUILD_SCRIPT),
            "-Version",
            version,
        ],
        cwd=source_root,
        check=False,
    )
    if result.returncode != 0:
        raise RuntimeError(f"build.ps1 failed with exit code {result.returncode}.")

def main() -> int:
    print("ReSkatePlus upstream builder (Automated Edition)\n")
    config = load_toml()
    latest = get_latest_commit(UPSTREAM_REPO)
    print(f"Upstream: {latest['repo']} ({latest['branch']})")
    print(f"Latest:   {latest['sha'][:12]} - {latest['date']}")
    print(f"Commit:   {latest['message'][0]}")
    WORK_DIR.mkdir(parents=True, exist_ok=True)
    source_root = download_checkout(latest)
    print("\nApplying patches...")
    count = apply_patches(source_root, config)
    print(f"Validated and applied {count} patches.")
    base_version = get_upstream_latest_version(UPSTREAM_REPO)
    version_file = SCRIPT_DIR / "current.version"
    version_file.write_text(base_version, encoding="utf-8")
    print(
        f"\nWorkspace setup completed. Recorded commit "
        f"{latest['sha'][:12]}. Tracking Upstream version: {base_version}"
    )
    return 0

if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (
        PatchError,
        RuntimeError,
        urllib.error.URLError,
        OSError,
        zipfile.BadZipFile,
    ) as exc:
        print(f"\nERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
