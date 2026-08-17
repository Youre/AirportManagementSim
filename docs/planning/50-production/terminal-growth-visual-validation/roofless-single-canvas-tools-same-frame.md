# Single Canvas Terminal Visual Comparison

- Reference: C:\Users\dave\Documents\AirportManagementSim\AMSim\Saved\SingleCanvasTerminalAudit\01-overview-roofless-terminal.png
- Capture: C:\Users\dave\Documents\AirportManagementSim\AMSim\Saved\SingleCanvasTerminalAudit\02-terminal-tools-same-frame.png
- Comparison board: C:\Users\dave\Documents\AirportManagementSim\docs\planning\50-production\terminal-growth-visual-validation\roofless-single-canvas-tools-same-frame.png
- Equivalent state: VisualBaseline at identical camera framing, before and after opening contextual Terminal tools

## Review checklist

- [x] The same airport geometry and camera framing remain behind both states.
- [x] Primary, secondary, and contextual actions have a clear hierarchy.
- [x] Palette, surface treatment, outlines, and depth cues form one visual language.
- [x] Typography size, weight, casing, and spacing are coherent.
- [ ] Contextual panels still consume more horizontal world area than desired.
- [x] Operational state remains legible without relying on color alone.
- [x] Mouse cursor remains visible and interactive in the gameplay viewport.

## Highest-impact mismatches

1. The 30-by-18-meter spatial terminal is present and roofless near the lower
   center, but it is too small to read at the full-airport overview scale.
2. Opening both contextual panels covers substantial left/right world area even
   though it no longer changes the camera or renderer.
3. The immediate terminal campus lacks the curb, apron, gate, and activity
   composition needed to make VA-03 recognizable at useful player zooms.

## Intentional differences

- The contextual tools and passenger inspector intentionally overlay the live
  world; they do not constitute a destination or alternate airport view.
- Player pan and zoom are intentionally unchanged when the panels open or close.
- The terminal is intentionally roofless in both states.

## Next bounded iteration

- Target: terminal campus scale and composition against VA-03 without adding a
  second renderer or facility-owned camera.
- Expected visual change: a readable terminal, curb, apron, gates, and access
  context at a player-selected intermediate zoom.
- Acceptance observation: the player pans and zooms one camera from the whole
  airport to the terminal interior; all objects retain one world position and
  opening Terminal tools changes only UI chrome.
