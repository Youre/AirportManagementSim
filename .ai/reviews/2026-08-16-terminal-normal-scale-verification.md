# Terminal normal-scale verification

Date: 2026-08-16
Result: Passed with continuing VA-01/VA-03 density polish

## Verified outcome

- The schema-10 terminal remains permanently roofless on the one authoritative
  Paper2D airport canvas.
- One shared geometry contract maps each saved logical cell to an eight-meter
  presentation module. The 18x12 starter layout presents at approximately
  144x96 meters; the 30x18 grown layout presents at 240x144 meters.
- Furniture, occupants, bags, vehicles, and construction workers use a bounded
  three-times readability scale rather than inheriting the facility multiplier.
- Rendering, placement previews, physical-size feedback, and pointer
  deprojection use the same exported geometry constants.
- A presentation-only 60-meter landside-campus offset places the grown
  footprint south of the access road. It changes no simulation state, save
  coordinate, camera location, or renderer ownership.

## Visual evidence

- Packaged implementation capture:
  `docs/planning/50-production/terminal-growth-visual-validation/normal-scale-terminal-packaged.png`.
- Equivalent-state VA-01 board and assessment:
  `docs/planning/50-production/terminal-growth-visual-validation/normal-scale-terminal-va01-comparison.png`
  and its adjacent Markdown worksheet.
- The first eight-meter package was rejected because its fixed center caused a
  road overlap. The retained recapture proves the bounded campus correction.

The terminal is now readable and physically credible at full-airport overview
zoom. VA-01/VA-03 still carry richer campus dressing, occupant density, route
hierarchy, and material depth; those remain bounded polish rather than scale or
renderer defects.

## Automated, package, and release evidence

- UE 5.8 Editor and game builds: passed.
- Focused
  `AMSim.TerminalGrowth.Presentation.RevisionGatingAndProxyReuse`: passed.
- Complete `AMSim` automation: 82 success, zero failed or not-run.
- Clean Development and Shipping `BuildCookRun`: passed, 750 cooked packages
  each, archived under
  `D:/AMSim-TerminalNormalScale8mCampus-20260816`.
- Packaged Phase 4 deterministic visual fixture: passed and exited normally.
- Phase 7 release audit: passed with zero runtime string loads, forbidden
  runtime dependencies, required 3D candidates, secret/endpoint candidates,
  missing Phase 7/UI-audio assets, or line-limit violations.
- Canonical Development launcher: `D:/AMSim-Current/Windows/AMSim.exe`; direct
  windowed launch smoke passed and the process was stopped normally afterward.

## Save and cache safety

- All 105 pre-existing canonical Saved files were preserved byte-for-byte.
- Recoverable backup:
  `D:/AMSim-Current/Windows-backup-terminal-normal-scale8m-20260816-201728`.
- Canonical inner executable SHA-256:
  `3CB9905D4EE8C598FF10A3A807DEF2BBEEB5BDF97254EC08202A81F125EFDFDD`.
- Cache status remained healthy: C free 126.71 GiB; D free 673.09 GiB;
  project caches 8.63 GiB; shared Unreal caches 0.81 GiB. No cache was cleared.

Windows Security presented a firewall prompt during an earlier automated visual
replay. No security setting or prompt was changed by the agent. Final visual
evidence came from the non-mutating deterministic packaged fixture, and the
canonical executable subsequently passed a direct responsiveness smoke.
