u51+# Copilot Instructions

## Project Guidelines
- For mod dumps, the user wants only EBX assets that are new or changed relative to the game, saved with original asset paths under a directory named after the selected mod (e.g. TestDumpMod/items/...ebx). Omit resources, chunks, generated GUID/hash/manifest files, and unrelated source files, because the user imports/replaces EBX assets in ReSkate Studio.
- For merge_targets.txt entries, include the mod name followed by the EBX root type in square brackets, before the EBX asset filename.

## Bug Fixing Guidelines
- When fixing reported bugs, add regression coverage that runs the actual production function against the exact reported input/error before reporting completion, rather than relying only on compilation or mock-like checks.

## Cosmetic Visibility Guidelines
- When assessing cosmetic visibility, distinguish ReSkate+'s local `reserved_cosmetic()` bypass (always false) from stock ReSkate's rule: paid cosmetics are reserved unless the viewer is identity Homie rank or higher. Do not infer stock-client behavior from the ReSkate+ override.