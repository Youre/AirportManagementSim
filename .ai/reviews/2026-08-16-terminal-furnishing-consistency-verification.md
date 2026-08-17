# Terminal furnishing consistency verification

Date: 2026-08-16
Result: Passed with continuing terminal-density polish debt

## Implemented correction

- Replaced the repeated, pre-baked seating-cluster presentation with a
  project-owned strict-nadir single-seat PaperSprite.
- Composed `SeatGroup2`, `SeatGroup4`, and `SeatGroup6` as exact 2/4/6-seat
  arrangements while preserving the authoritative anchor and quarter turn.
- Made the second row of back-to-back groups face the opposite direction.
- Gave seats their own reusable presentation proxy pool.
- Fit every non-seat furnishing uniformly inside its rotated footprint so
  rectangular layouts no longer distort source art.
- Left Phase 3 terminal state, schema 10, routing, capacity, readiness,
  construction, and the Phase 1 economy ledger unchanged.

## Asset and provenance proof

- Source master:
  `SourceAssets/TerminalGrowth/Masters/OpenAI/T_SeatSingle_OpenAI_Master.png`
- Runtime source:
  `SourceAssets/TerminalGrowth/Runtime/Textures/T_SeatSingle.png`
- Runtime SHA-256:
  `5BBE39F62DA65E437C4A7C7CA13C8031982071578CBA6762E14D04988DF3738C`
- Cooked assets:
  `/Game/TerminalGrowth/Presentation/Textures/Furniture/T_SeatSingle` and
  `/Game/TerminalGrowth/Presentation/Sprites/Furniture/S_SeatSingle`
- The generated runtime manifest contains 45 retained TerminalGrowth assets.
- Faux-perspective and background-bearing generated candidates were rejected
  and remain outside the repository.

## Automated verification

- Focused `AMSim.TerminalGrowth.Presentation.RevisionGatingAndProxyReuse`:
  passed.
- Complete `AMSim` automation: 82/82 succeeded, zero failed, not-run, or
  in-process tests. The known backup-recovery read warning remains expected.
- Added checks cover exact visible seat count, cooker-visible sprite,
  aspect-preserving scale, opposite-facing rows, and proxy reuse.
- `AMSimEditor Win64 Development` and `AMSim Win64 Development` builds passed.

## Package and release verification

- Clean Development archive:
  `D:/AMSim-TerminalFurnishing-20260816/Development`
- Clean Shipping archive:
  `D:/AMSim-TerminalFurnishing-20260816/Shipping`
- Both package configurations passed BuildCookRun and contain 750 cooked
  packages. The cooked reference set includes both SeatSingle packages.
- Packaged Phase 7 runtime checks passed: Development journey/save-load
  continuity and Shipping offline launch both succeeded.
- Release audit passed with zero runtime string asset loads, forbidden runtime
  dependencies or package matches, required 3D candidates, line-limit
  violations, or missing required release assets.
- The verified Development archive refreshed
  `D:/AMSim-Current/Windows/AMSim.exe`; all 91 existing canonical Saved files
  remained byte-identical. Canonical inner binary SHA-256:
  `9810651D81B8F3C6D7F4EE2D0A448D7DEEE43B941B8E0D0473661A60DD79FD47`.

## Visual verification

- Before capture:
  `AMSim/Saved/TerminalFurnishingAudit/02-current-terminal-furnishing-build.jpg`
- Corrected canonical capture:
  `AMSim/Saved/TerminalFurnishingAudit/07-canonical-corrected-terminal.png`
- Before/after board:
  `AMSim/Saved/TerminalFurnishingAudit/05-before-after-furnishings.png`
- VA-03 equivalent-state board:
  `AMSim/Saved/TerminalFurnishingAudit/08-va03-canonical-furnishings.png`

The corrected package visibly distinguishes two-, four-, and six-seat groups,
keeps seat and prop silhouettes in strict top-down view, and removes footprint
stretching. VA-03 still has materially more counters, queue furniture, baggage
equipment, passengers, and integrated route detail. That is continuing content
density work, not a defect in the corrected furnishing composition contract.

## Environment

- C retained 128.12 GiB free and D retained 687.44 GiB free at closeout.
- No project or shared Unreal cache was cleared.
- Unreal canvas content exposes no useful child accessibility tree through
  Windows UI Automation; interaction and visual evidence therefore combine
  current packaged screenshots, deterministic fixtures, and engine automation.
