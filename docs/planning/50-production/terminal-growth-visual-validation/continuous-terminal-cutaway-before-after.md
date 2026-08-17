# Continuous terminal cutaway remediation Visual Comparison

- Reference: C:\Users\dave\Documents\AirportManagementSim\AMSim\Saved\ProceduralOverviewAudit\03-packaged-terminal-cutaway.png
- Capture: C:\Users\dave\Documents\AirportManagementSim\AMSim\Saved\ContinuousTerminalAudit\02-same-frame-terminal-cutaway.png
- Comparison board: C:\Users\dave\Documents\AirportManagementSim\docs\planning\50-production\terminal-growth-visual-validation\continuous-terminal-cutaway-before-after.png
- Equivalent state: Loaded VisualBaseline with terminal operations open: replacement terminal screen versus contextual cutaway on the airport map.

## Review checklist

- [x] World versus UI dominance matches the continuous-airport intent.
- [x] Primary, secondary, and contextual actions have a clear hierarchy.
- [x] Palette, surface treatment, outlines, and depth cues remain in one visual language.
- [x] Typography size, weight, casing, and spacing remain coherent with the root shell.
- [x] Contextual panels leave the airport world dominant and scannable.
- [x] Operational state remains legible without relying on color alone.
- [x] Mouse cursor remains visible and interactive in the gameplay viewport.

## Highest-impact mismatches

1. The former Terminal action collapsed the root airport page and substituted a terminal-specific header, footer, and side chrome.
2. Enabling cutaway hid mature airport surfaces, aircraft, landscaping, and activity; a hidden regional presenter could also reassert the wrong visibility state during refresh.
3. The former camera contract jumped to a terminal-centered frame and restored a separately saved management frame on close, making the terminal feel like another scene.

## Intentional differences

- Contextual terminal tools still occupy bounded side regions while the cutaway is active; they are overlays on the persistent root shell, not a replacement screen.
- The terminal interior is intentionally small at the complete-airport zoom and becomes readable through ordinary player-controlled zoom, preserving true airport scale.

## Next bounded iteration

- Target: Continuous airport-world terminal cutaway.
- Expected visual change: Opening Terminal changes only the terminal roof/interior and adds contextual tools while preserving the full airport and exact camera frame.
- Acceptance observation: The identical-frame before/after capture retains runway, apron, roads, aircraft, landscaping, root chrome, and camera position; zoom/pan remains available and closing does not snap the map.
