# Build-world visual recovery verification

Date: 2026-08-16
Result: Passed; owner visual acceptance remains the next human gate

## Outcome

- Restored the VA-02 planning grid, parcel boundary, translucent proposal
  surface, outline/pattern treatment, and distinct placement-node language in
  the single Paper2D airport-world renderer.
- Centralized height and translucent-sort ordering for terrain, construction
  beds, roads, runways, taxiways, gates, structures, terminal roof, proposal
  patterns, and interaction markers.
- Kept simulation, schemas, costs, snapping rules, construction timing, and
  committed geometry unchanged.

## Automated verification

- `AMSim.TerminalGrowth.Presentation.RevisionGatingAndProxyReuse`: passed after
  adding grid/pattern activation, marker-asset, layer-order, pool-reuse, and
  clear-on-exit assertions.
- Complete `AMSim` Unreal automation: 81 of 81 passed with zero failed,
  not-run, or in-process results. Log:
  `AMSim/Saved/Logs/BuildWorldVisualRecovery-FullAutomation.log`.
- UE 5.8 Win64 Development game build: passed after replacing the editor-only
  PaperSprite source-size query with runtime-safe render bounds.
- Phase 7 release audit: passed with zero runtime string asset loads, forbidden
  runtime dependencies, required 3D assets, secrets/endpoints, or source files
  above 2,000 lines.

## Visual verification

- Replayed the starter-airfield build journey at 1280x720 in Unreal.
- Confirmed large, distinct runway and gate connection nodes while the taxiway
  tool was active.
- Drew a runway-to-gate taxiway and reached the ordinary `Network Ready`
  validation state.
- Deliberately drew a service road through both gate footprints. Gate artwork
  remained above the road while endpoint/interaction markers remained above
  the gates.
- Compared the identical implementation state against VA-02 on one board and
  recorded remaining density, catalog, and inspector differences in
  `docs/planning/50-production/build-world-visual-recovery/va02-comparison.md`.

## Packaging and save preservation

- Clean Development BuildCookRun: passed, 748 cooked packages, archive at
  `D:/AMSim-Current/Windows`.
- Canonical launcher SHA-256:
  `8E61AE1487664610F75BCC9A2439DCBD48541F8CEEAEF88A913BF9EE5A2F0CD5`.
- Canonical inner executable: 227,872,768 bytes; SHA-256
  `DC08F86355C2DEBC19A137C7E220B4AEA9889B0E516F7A2BEE8D27A65F61879C`.
- Cook reference inventory contains the translucent Paper2D preview material
  and all four retained placement-marker sprites.
- The packaged Development executable remained responsive for eight seconds.
  Its only TCP observation was the expected local Development trace listener
  on port 1985; it opened no remote connection.
- All 24 pre-existing canonical saved files were restored and verified by
  SHA-256 with zero differences after the package refresh and smoke.

## Cache observation

- Before packaging: project 8.59 GiB, shared Unreal cache 0.80 GiB, C: 129.90
  GiB free, D: 696.50 GiB free.
- After packaging: project 27.10 GiB including project-local build products,
  shared Unreal cache 1.09 GiB, C: 129.64 GiB free, D: 696.51 GiB free.
- Free-space thresholds remained healthy. No shared DDC, Zen, or UBA cache was
  cleared.

## Remaining intentional visual debt

- The starter state is much less dense than the mature VA-02 airport.
- The three-tool starter catalog remains text-based until the construction
  inventory grows enough to justify the reference's thumbnail treatment.
- The proposal inspector exposes supported facts but not yet the reference's
  richer metric and surface-thumbnail treatment.
