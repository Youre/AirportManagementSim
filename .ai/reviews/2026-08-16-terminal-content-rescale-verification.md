# Terminal content rescale verification

Date: 2026-08-16
Result: Passed with continuing material/activity polish

## Verified outcome

- Static furniture uniformly fits within 84 percent of the full authored
  sixteen-meter presentation-cell footprint.
- Exact two-, four-, and six-seat compositions use the same full-cell spacing
  and retain their saved proxy counts and opposing-row orientations.
- Passengers, visitors, construction workers, bags, staff, and service vehicles
  use a distinct twelve-times readability scale.
- Object anchors, rotations, room boundaries, routes, occupancy, schema,
  simulation checksums, world renderer, camera, and pointer mapping are
  unchanged.

## Visual evidence

- Packaged capture:
  `docs/planning/50-production/terminal-growth-visual-validation/scaled-content-terminal-packaged.png`
- Identical-state before/after:
  `docs/planning/50-production/terminal-growth-visual-validation/scaled-content-terminal-before-after.png`
- VA-01 comparison:
  `docs/planning/50-production/terminal-growth-visual-validation/scaled-content-terminal-va01-comparison.png`

The before/after demonstrates that the prior dot-sized furniture has become
legible seating, desks, amenities, passengers, bags, staff, and vehicles at the
same 1920x1080 camera frame. Contents remain inside their room and route
context. Remaining VA-01/VA-03 differences are material hierarchy, route
treatment, and overall activity density rather than scale.

## Technical verification

- Editor and Development game builds: passed.
- Focused terminal presentation automation: passed.
- Complete `AMSim` automation: 82 succeeded, 0 failed.
- Deterministic packaged Phase 4 journey: passed; screenshots, save/load,
  state, performance, and reconciliation assertions passed.
- Clean Development and Shipping BuildCookRun: passed, 750 cooked packages.
- Phase 7 release audit: passed with zero runtime string loads, forbidden
  runtime dependencies, required 3D candidates, or line-limit violations.
- Canonical windowed launch smoke: passed.
- Canonical inner SHA-256:
  `E9F70D254097C9CA91985A81721DA606614C45605534BA5E3EA309E713B9C4B2`
- All 106 pre-refresh canonical Saved files were restored byte-for-byte after
  the smoke; generated smoke deltas remain recoverable in the package backup.

## Storage discipline

- C free: 125.05 GiB.
- D free: 669.58 GiB.
- Project caches: 8.61 GiB.
- Shared Unreal caches: 0.81 GiB.
- No cache was cleared.
