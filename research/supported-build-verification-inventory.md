# Supported-build verification inventory

This inventory describes the current source on the `research/rva-usage-telemetry-2.2.0` branch. It distinguishes whole-build identity gates from local feature fingerprints. It is not an exhaustive inventory of every local byte guard in the repository.

## Accepted build identity

`Engine/Game/Build/supported_build.h` remains the authoritative accepted-build contract. It pins the Skate executable size, PE image size and SHA-256; Steam API DLL size and SHA-256; Steam app/depot/manifest/build identifiers; Data changelist; world-layer count; and content-cache identity.

**No new Skate executable build has been accepted by this branch.** The address namespace remains `Engine/Game/Build/20260929/`. ReSkatePlus product version 2.2.0 is not evidence that a different Skate executable is compatible.

## Whole-build gates

| Location | Check | Effect |
|---|---|---|
| `Engine/Core/Platform/launcher_pe.cpp::validate_game_file` | Exact executable file size, AMD64 PE identity, expected image size, and SHA-256 | Rejects an unrecognized Skate.exe before launch |
| `Engine/Core/Platform/launcher_pe.cpp::check_steam_api_file` | Steam API DLL size, x64 PE DLL identity, and SHA-256 | Rejects an unexpected Steam API library |
| `Launcher/launch.cpp::game_files_supported` | Runs executable and Steam API validation, then validates game data | Launcher refuses unsupported or damaged game folders |
| `Launcher/launch.cpp::game_data_mismatched` | Requires Data/layout.toc and a readable changelist equal to the pinned changelist; this branch rejects both unknown and mismatched changelists | Prevents an unknown or mixed data build from passing based only on file sizes |
| `Launcher/launch.cpp::game_data_damage` | Checks manifest-listed Data files for presence, expected size, and cloud-placeholder/offline-only attributes | Rejects incomplete or unavailable game data |
| `Launcher/launch.cpp::config_matches_build` | Compares updater configuration's Skate SHA-256, Steam app ID, depot ID, and manifest ID against the pinned contract | Checks the build an update configuration describes; installed files are still validated independently |
| `Launcher/launch.cpp::validate_child_image` | Confirms the suspended child is the same file validated by the launcher; checks loaded AMD64 PE headers, image size, client-tick prefix, and all central image checks | Refuses injection if the created process image does not match |
| `Runtime/runtime_init.cpp::validate_image` | Calls `supported_build::running_image_matches` before PE/header and central fingerprint checks | Rechecks the executable SHA-256 at runtime so direct/manual DLL loading cannot bypass the launcher gate |

`Launcher/launch.cpp::configure_environment` sets `RESKATE_LOG_DIRECTORY` for the child process. The development tracer uses that existing path; no new environment variable or log-directory convention is required.

## Runtime image identity helper

`Engine/Game/Build/image_identity.cpp::image_headers_match` validates the loaded main-image PE headers. `running_image_matches` also confirms that the supplied base is the process main image and hashes the executable on disk against the pinned SHA-256. The runtime calls this helper before initializing game hooks.

## Local feature guards

Build-specific headers under `Engine/Game/Build/20260929/` hold function fingerprints, byte prefixes, data RVAs, patch sites, and related contracts. Feature initialization code may perform additional local checks immediately before installing a hook or patch. For example, `Extension/World/level_loading.cpp` checks the transition/caller contract, level-request contract, and disconnect contract before installing its observers.

These local guards supplement the whole-build gate; they do not replace it. An exhaustive repository-wide list of all local guards remains future audit work.

## Safety policy

1. Keep the existing executable, Steam API, Data, depot, manifest, and content-cache pins unchanged while collecting diagnostics.
2. Unknown executable hashes, unknown/unreadable Data changelists, and mixed or incomplete game folders must fail closed.
3. Do not add a best-effort path based only on image size, a few signatures, or a build-number string.
4. A new Skate build needs its own reviewed executable/data identity contract and matching address/fingerprint set before any RVA-dependent code is enabled.
5. Keep feature-level fingerprints in place after a build passes the whole-build gate.
6. Treat telemetry as observations about instrumented call sites only. Do not infer behavior or equivalence from an RVA hit, event count, or matching bytes alone.
