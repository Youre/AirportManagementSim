# Visual-baseline save and HUD remediation preliminary adversarial review

Date: 2026-08-16
Status: Approved with controls below

## Failure cases and controls

1. **The installer overwrites the owner's airport.** It targets only the fixed
   `VisualBaseline` slot, refuses an existing target by default, and requires an
   explicit refresh switch that first creates a recoverable backup.
2. **The script guesses the runtime save root.** It derives the portable
   package's project `Saved` directory from the verified canonical inner
   executable and verifies that the ordinary Load menu lists the installed
   slot.
3. **The baseline becomes a hand-maintained binary that silently goes stale.**
   The script regenerates it from the deterministic Phase 4 fixture and records
   package identity, schema, snapshot hash, fixture result, and generation time.
4. **A development fixture leaks into a normal new game.** Installation is an
   explicit project script; runtime startup and Shipping content do not seed it.
5. **The visual audit compares unrelated states.** Before and after evidence
   loads the same installed snapshot, uses the same screen, viewport, UI scale,
   and camera, and records those facts beside the board.
6. **Visual cleanup removes needed controls.** Save/load, time control, help,
   destination navigation, alerts, flights, projects, and selected-object
   actions remain reachable and receive interaction tests.
7. **The footer merely hides clutter behind unlabeled icons.** Grouped controls
   retain labels/tooltips/focus states and use the project icon family; compact
   mode uses drawers rather than shrinking text.
8. **The concept art overrides gameplay truth.** Written PX-03/PX-04/CT-05
   requirements control content and actions; VA-01 controls hierarchy, density,
   shape, tone, and world/UI balance.
9. **The HUD change reintroduces a widget-local airport renderer.** Only UMG
   chrome changes. All world geometry remains in `AAMSimWorldPresenter`.
10. **The pass edits the near-limit root file beyond the 2,000-line standard.**
    New reusable composition helpers move to focused source files; the root
    implementation must remain within the repository limit.
11. **A baseline load corrupts deterministic state.** Generation and installed
    copies are hash-checked; ordinary restore validation and post-load checksum
    tests must pass.
12. **A clean package erases the new baseline.** The canonical package refresh
    backs up and restores the verified package-side save root, then verifies
    slot hashes after archive replacement.
13. **A supported older save is invisible before migration.** Metadata
    validation accepts schemas 1 through the current version for discovery;
    ordinary restore remains responsible for migration and future schemas are
    still rejected.

## Required proof

Do not close the iteration from screenshots alone. Require a real packaged load
journey, same-state VA-01 board, design QA with no remaining P0-P2 findings in
the bounded HUD system, focused/full automation, clean canonical package,
runtime dependency scan, and exact LocalAppData save preservation.
