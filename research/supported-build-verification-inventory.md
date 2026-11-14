# Supported-build verification inventory

Audited against `main` at the branch point for `research/rva-usage-telemetry`. This inventory separates whole-build gates from local fingerprint guards. It is not permission to weaken or bypass any check.

## Whole-build identity and launch gates

| Location | Check | Consequence |
|---|---|---|
| `Engine/Game/Build/supported_build.h` | Pins Skate.exe file size, PE image size, SHA-256, Steam API DLL size/hash, Steam app/depot/manifest/build IDs, Data changelist, world-layer count, and content-cache identity. | Authoritative accepted-build contract. Do not update these pins until a new build's code and data contracts have been reviewed. |
| `Engine/Core/Platform/launcher_pe.cpp::validate_game_file` | Requires exact Skate.exe file size, AMD64 PE executable identity, expected image size, and exact SHA-256. | Rejects an unrecognized executable before launch. |
| `Engine/Core/Platform/launcher_pe.cpp::check_steam_api_file` | Requires exact steam_api64.dll size, x64 PE DLL identity, and exact SHA-256. | Rejects an unexpected Steam API library. |
| `Launcher/main.cpp` calls `launcher_app::game_files_supported` before starting the game. That function in `Launcher/launch.cpp` calls the executable and Steam DLL validators, then checks game data. | Launcher refuses unsupported or damaged game folders. |
| `Launcher/launch.cpp::game_data_mismatched` / `game_data_damage` | Requires Data/layout.toc, checks its pipeline changelist against `game_data_changelist`, then checks all 98 manifest-listed Data files for existence, exact size, and cloud-placeholder/offline-only attributes. | Rejects mixed-build, missing, truncated, or online-only game data even if Skate.exe itself is the pinned build. |
| `Launcher/launch.cpp::config_matches_build` (declared in `Launcher/launch.h`) | Compares updater config's Skate SHA, Steam app ID, depot ID, and manifest ID against the pins. | The helper exists, but this audit did not find a production call site; do not count it as an enforced launch gate until its caller is confirmed. |
| `Launcher/launch.cpp::validate_child_image` | After creating Skate.exe suspended, confirms the child image is the same file validated by the launcher, checks loaded AMD64 PE headers and image size, then checks `runtime::client_tick_prefix` and every entry in `runtime::image_checks`. | Refuses injection before ReSkate.dll is loaded if the actual child image does not match the expected code anchors. |
| `Runtime/runtime_init.cpp::validate_image` | Calls `supported_build::running_image_matches` first (exact on-disk SHA-256 plus main-image/PE-header check), then independently checks DOS/NT headers, AMD64 machine, image size, client-tick prefix, and all `runtime::image_checks`. | ReSkate.dll refuses startup before runtime hooks and feature initialization if the executable hash or loaded-image anchors fail. This explicit runtime hash call is added on this branch so manual/direct DLL loading cannot bypass the launcher's file hash gate. |

## Identity helper whose active call site still needs confirmation

`Engine/Game/Build/image_identity.cpp::image_headers_match` checks loaded PE headers. `running_image_matches` additionally hashes the executable on disk and checks that the supplied base is the process main image. The helper is now called by `Runtime/runtime_init.cpp::validate_image` on this branch, before runtime initialization. It confirms the process main image and hashes the executable on disk against the pinned SHA-256 (cached once per process).

## Per-feature fingerprint and patch guards

The central runtime image checks above are not the only protections. Build-specific headers under `Engine/Game/Build/20260929/` contain function fingerprints, byte prefixes, data RVAs, patch sites, and related contracts. Feature initialization code often checks its own expected bytes immediately before preparing/enabling a hook or applying a patch.

For example, `Extension/World/level_loading.cpp`:
- `compatible` checks the level-transition function and caller-contract fingerprints before installing the transition hook.
- `trace_state_requests` checks the level-request function fingerprint before installing its hook.
- `trace_disconnects` checks the disconnect-function fingerprint before installing its hook.

These are local feature guards, not proof of whole-build identity. A full exhaustive list of every such guard across all Extension and Engine sources remains an audit task; this document does not claim that list is complete.

## Safety policy for the newer-build branch

1. Keep the existing 20260929 SHA/data/depot pins and all current global gates intact while adding diagnostics.
2. Unknown executable hashes, unknown Data changelists, and mixed/partially updated folders must fail closed before any game RVA is called or hook/patch is installed.
3. Do not make a "best effort" path that merely checks image size, a few signatures, or a build-number string and then continues.
4. A future explicitly supported build needs its own reviewed identity contract and matching address/fingerprint set. A candidate build that has not been researched must remain rejected.
5. Feature-level byte checks remain in place after a build is accepted; they protect individual hooks/patches against stale or incorrect local assumptions.

## Telemetry scope

The first development-only tracer records entry/exit for the existing level-transition, level-request, and connection-disconnect hooks. It is a proof of concept for the logging format and selected state snapshots, not yet a complete census of all addresses in the 137 build headers. Direct calls, globals, vtables, layouts, and patch sites need different observation strategies and must be inventoried separately.
