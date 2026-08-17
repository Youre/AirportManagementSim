# Continuous Terminal Cutaway Verification

**Date:** 2026-08-16 EDT
**Branch:** `codex/planning-docs-concept-art`
**Commit baseline:** `3bc9e2e27f15f953a99548563124606bea8e10cb`
**Result:** Passed with continuing VA-03 visual-polish debt

## Verified contract

- Terminal opens as a contextual overlay on the existing airport canvas.
- The persistent airport header, footer, and side rails remain present.
- Runway, apron, roads, aircraft, landscaping, activity, and procedural mature
  facilities remain visible while the terminal interior is exposed.
- The generated terminal roof and schema-10 terminal interior are the two
  states of the same world object; no replacement terminal scene is rendered.
- Opening the cutaway preserves the exact camera position and orthographic
  width. Player pan and zoom remain available, and closing preserves the
  current camera rather than restoring a separately saved terminal frame.
- Hidden destination presenters do not overwrite terminal visibility state.

## Evidence

- Exact-frame closed state:
  `AMSim/Saved/ContinuousTerminalAudit/01-airport-frame-roof-closed.png`
- Exact-frame open state:
  `AMSim/Saved/ContinuousTerminalAudit/02-same-frame-terminal-cutaway.png`
- Open cutaway after player zoom and pan:
  `AMSim/Saved/ContinuousTerminalAudit/03-terminal-cutaway-zoom-and-pan.png`
- Before/after board:
  `docs/planning/50-production/terminal-growth-visual-validation/continuous-terminal-cutaway-before-after.png`
- VA-03 board:
  `docs/planning/50-production/terminal-growth-visual-validation/continuous-terminal-cutaway-va03.png`

The exact-frame pair retains the same exterior airport composition and camera
frame. The open image changes terminal roof/interior state and adds only
contextual terminal controls.

## Automated and package verification

- Complete Unreal automation: 82 discovered, 82 Success, zero non-success.
- Clean Development BuildCookRun: passed at
  `D:/AMSim-ContinuousAirportCanvasV2-20260816/Development/Windows/AMSim.exe`.
- Clean Shipping BuildCookRun: passed at
  `D:/AMSim-ContinuousAirportCanvasV2-20260816/Shipping/Windows/AMSim.exe`.
- Phase 7 release audit: passed with 750 cooked packages, zero runtime string
  asset loads, zero forbidden runtime dependencies, zero required 3D asset
  candidates, and zero line-limit violations.
- Canonical inner Development executable SHA-256:
  `271D4460E214962D0750C731219FAC23F89F0DC8552EDEC2B4F8AB48C920058C`.

## Manual interaction evidence and limits

The packaged canonical build was used to load `VisualBaseline`, open Terminal,
zoom, pan, close, and compare the same airport frame. Mouse clicking, keyboard
pan, wheel zoom, and visible cursor behavior were observed. Windows UI
Automation exposes the Unreal window rather than semantic child controls, so
this pass does not claim screen-reader labels or accessibility-tree coverage.

## Remaining visual debt

The architecture and continuity defect is corrected, but visual parity with
VA-03 is not complete. The close terminal view still needs denser strict-nadir
furniture and occupants, stronger room and route hierarchy, and richer curb,
gate, apron, and landscape treatment. Those changes must retain this single-
canvas contract.

No Unreal cache was cleared. At closeout, C retained 127.77 GiB free and D
retained 681.67 GiB free.
