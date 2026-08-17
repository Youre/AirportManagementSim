# Terminal Double-Scale Verification

**Date:** 2026-08-16 EDT
**Result:** Passed with continuing visual-density polish
**Scope:** Presentation geometry only; no simulation, schema, save, or camera change

## Verified outcome

- The shared terminal presentation module is 16 meters per logical cell.
- The 18x12 starter terminal presents at approximately 288x192 meters.
- The 30x18 grown terminal presents at approximately 480x288 meters.
- Furniture, occupants, bags, and vehicles use the matching bounded six-times
  readability scale.
- Renderer and pointer deprojection consume the same geometry constants.
- The fixed campus anchor places the grown terminal between the apron and
  landside access road. Automation guards the road clearance.
- The terminal remains permanently roofless in the authoritative Paper2D
  airport world. No terminal camera or alternate render path was introduced.

## Visual evidence

- Packaged source capture:
  `docs/planning/50-production/terminal-growth-visual-validation/double-scale-terminal-packaged.png`
- Equivalent-state VA-01 board:
  `docs/planning/50-production/terminal-growth-visual-validation/double-scale-terminal-va01-comparison.png`
- Review worksheet:
  `docs/planning/50-production/terminal-growth-visual-validation/double-scale-terminal-va01-comparison.md`

At 1920x1080 the grown terminal occupies roughly one quarter of the world
width, keeps apron and landside context visible, and no longer reads as a
floor-plan thumbnail. The remaining gap to VA-01 is architectural, material,
and activity density rather than footprint scale.

## Technical verification

- `AMSim.TerminalGrowth.Presentation.RevisionGatingAndProxyReuse`: passed.
- Complete `AMSim` automation: 82 discovered, 82 succeeded, 0 failed.
- Clean Development BuildCookRun: passed, 750 cooked packages.
- Clean Shipping BuildCookRun: passed, 750 cooked packages.
- Deterministic packaged Phase 4 journey: completed and exited normally.
- Phase 7 release audit: passed.
  - runtime string asset loads: 0
  - forbidden runtime dependencies: 0
  - required 3D asset candidates: 0
  - line-limit violations: 0
- Canonical Development package: real windowed launch smoke passed.
- Canonical inner executable SHA-256:
  `26D4725C40C3051D168C53702F3CA76B31557796366E20C7DF63FBDE0755135B`
- All 106 pre-refresh files under the canonical `Saved` directory were restored
  byte-for-byte after the smoke; smoke-generated log deltas were retained in
  the recoverable package backup.

## Storage discipline

- C: retained 125.06 GiB free.
- D: retained 671.31 GiB free.
- Project Intermediate: 8.32 GiB.
- Shared Unreal caches: 0.44 GiB.
- No cache was cleared.

## Continuing debt

The next bounded visual pass should improve terminal-campus paths, surface
hierarchy, furnishing richness, and operational activity. It should not change
the terminal scale, renderer ownership, or camera contract again.
