# Roofless single-canvas terminal verification

Date: 2026-08-16
Status: Passed with continuing scale/composition follow-up

## Verified outcome

- The schema-10 interior renders whenever its layout is non-empty, independent
  of Terminal-tool state.
- Roof surface, perimeter-detail, corner, canopy, and HVAC proxies remain
  hidden in every gameplay mode.
- Opening and closing Terminal tools changes contextual UI and placement
  interaction only. It does not move, zoom, save, restore, or replace the
  management camera.
- One adaptive player-controlled zoom curve spans airport overview through
  close interior inspection.
- The fixed-coordinate Phase 3 proof renderer remains hidden while the mature
  airport and spatial terminal coexist.

## Evidence

- UE 5.8 Editor build: passed.
- Focused
  `AMSim.TerminalGrowth.Presentation.RevisionGatingAndProxyReuse`: passed.
- Complete `AMSim` automation: 82 success, zero fail, zero not-run.
- Clean Development and Shipping `BuildCookRun`: passed, 750 cooked packages
  each.
- Phase 7 release audit: passed with zero runtime string loads, forbidden
  editor/MCP/test dependencies, required 3D candidates, or line violations.
- Packaged Development replay:
  - `AMSim/Saved/SingleCanvasTerminalAudit/01-overview-roofless-terminal.png`
  - `AMSim/Saved/SingleCanvasTerminalAudit/02-terminal-tools-same-frame.png`
  - `AMSim/Saved/SingleCanvasTerminalAudit/03-close-tools-preserves-player-zoom.png`
- Canonical package: `D:/AMSim-Current/Windows/AMSim.exe`.
- Canonical inner executable SHA-256:
  `EC5D8103A1391993F14180DF020D873A84F9C7622680659E99D2C7D3C57824E4`.
- All 103 canonical saved files were preserved byte-for-byte. Recoverable
  backup:
  `D:/AMSim-Current/Windows-backup-roofless-single-canvas-20260816-192206`.
- Cache measurement remained healthy; no Unreal cache was cleared.

## Honest visual limit

At the full-airport overview scale, the physically scaled 30-by-18-meter
terminal occupies only a small portion of the map. The interior is present and
roofless, but its content becomes readable only after player-controlled pan and
zoom. Improving the terminal campus scale and composition against VA-03 remains
a separate visual iteration; it must not reintroduce a second world or
facility-owned camera.
