# September 29, 2026 build data

This directory contains the build-specific data for the 20260929 executable.

## Header split

Each feature area keeps its structural and behavioral declarations in `<area>.h`.
Build-specific executable addresses and signatures are isolated in the matching
`<area>_rvas.h` companion:

- Function, call-site, global, vtable, type-record, and import-slot RVAs.
- `Fingerprint` values, including their expected instruction bytes.
- RVA-bearing image checks and native patch address tables.

Non-RVA data stays in the feature header. This includes object/member offsets,
field sizes, flags, enum values, layout descriptions, and other constants that
are not executable addresses. Do not move a small numeric offset into an RVA
header merely because it is represented by `std::uintptr_t`.

Feature headers include their own companion file, so existing feature code
continues to use the same namespaces and names. `rvas.h` is the build-level
umbrella for inspecting or selecting the complete address/signature map.

## Adding another executable build

Create a sibling directory (for example, `20261013`) with the same companion
header names. Populate its RVA and fingerprint declarations only from evidence
for that executable. Keep structural offsets separate, and do not copy the
20260929 addresses or fingerprints as placeholders. A zero/unassigned address
must be treated as unavailable by the code that consumes it.
