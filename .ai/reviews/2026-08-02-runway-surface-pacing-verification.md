# Runway Surface and Construction Pacing Verification

Date: 2026-08-02
Status: Passed

## Outcome

The starter-airfield construction sequence now takes 210 game minutes: 30
minutes of delivery travel, 30 minutes of site preparation, 120 minutes of
visible surface work, and 30 minutes of inspection. With the established
one-game-minute-per-real-second mapping, this is 3 minutes 30 seconds at 1x or
26.25 seconds at 8x. A useful service road retains its 20 percent reduction,
making the corresponding times 2 minutes 48 seconds and 21 seconds.

The finished runway and taxiway now use distinct reviewed aggregate textures.
The runway has off-white edge, center, and threshold markings; the taxiway has
amber guidance markings; service roads and unfinished earthwork remain on the
separate unmarked sprite. Reciprocal 09/27 number overlays face their incoming
approaches instead of being rotated 180 degrees away from them.

## Asset evidence

- Local ComfyUI generated four runway and four taxiway material candidates.
- The retained unmarked masters, request IDs, seeds, checksums, prompts, and
  deterministic processing are recorded in
  `docs/planning/30-content-and-assets/phase-1-movement-surface-provenance.md`.
- Two-by-two seam sheets and final source textures were visually reviewed.
- The focused Unreal importer created exactly two textures and two Paper2D
  sprites; it did not rewrite unrelated presentation assets.
- Direct Development IoStore inspection found `T_RunwayMarked`,
  `S_RunwayMarked`, `T_TaxiwayMarked`, `S_TaxiwayMarked`, and both texture
  bulk-data entries.

## Verification evidence

- UE 5.8 Development editor build: passed.
- Focused fixture, progress-mapping, Paper2D asset, and runway-yaw coverage:
  3 of 3 tests passed.
- Complete `AMSim` automation: 73 of 73 tests passed with zero failures; the
  retained negative save probe emitted its expected missing-file warning.
- UE 5.8 Development game build: passed.
- Integrated 1920x1080 rendered smoke: passed journey, save/load, assertions,
  screenshots, and performance with zero 8x backlog.
- Visual review of delivery, midpoint construction, completed runway, and
  both approach directions: passed.
- Clean Development BuildCookRun: passed with 658 cooked packages.
- Packaged 1920x1080 smoke: passed journey, save/load, assertions, and
  screenshots with zero unexpected TCP connections.
- Runtime string asset-load scan: zero matches.
- Forbidden runtime dependency scan: zero matches.
- Source line-limit scan: zero files over 2,000 lines.
- Diff whitespace check: passed.

Package:
`D:/AMSimRunwaySurfaceDev-20260802-1627/Windows/AMSim.exe`

Cache tracking remained healthy. Before packaging, C: had 228.48 GiB free,
project caches were 8.76 GiB, and shared Unreal caches were 0.79 GiB. After
packaging, C: had 228.47 GiB free and both reported cache totals were unchanged
at two-decimal precision. No cache was cleared.
