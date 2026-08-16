# VA-01 Visual Comparison

- Reference: C:\Users\dave\Documents\AirportManagementSim\docs\planning\30-content-and-assets\concept-art\01-airport-overview-hud.png
- Capture: C:\Users\dave\Documents\AirportManagementSim\docs\planning\50-production\visual-baseline-remediation\audit\06-visual-baseline-airport-root-before.png
- Comparison board: C:\Users\dave\Documents\AirportManagementSim\docs\planning\50-production\visual-baseline-remediation\comparisons\va01-before.png
- Equivalent state: Established airport overview with active operations, schedule pressure, staffed services, and a selected aircraft

## Review checklist

- [ ] World versus UI dominance matches the reference intent.
- [ ] Primary, secondary, and contextual actions have a clear hierarchy.
- [x] Palette and rounded surface treatments remain recognizably Riverbend.
- [ ] Typography size, weight, casing, and spacing are coherent.
- [ ] Panel density and whitespace support quick scanning.
- [x] Operational state remains legible without relying on color alone.
- [x] Mouse cursor remains visible and interactive in the gameplay viewport.

## Highest-impact mismatches

1. The persistent 320 px objective/status/action rail replaces VA-01's compact activity destinations and turns diagnostic and command detail into permanent chrome.
2. Pause and speed controls sit in a bottom wall of unrelated save, destination, window, and help actions; VA-01 groups simulation status and time controls in the top bar and reserves the bottom for selection context.
3. The header is roughly twice the concept height because the brand, operating state, and economy wrap into multiple lines; the resulting world is smaller even though the airport art itself is readable.

## Intentional differences

- The current fixture is a flight-school/regional airport rather than the mixed GA airport illustrated in VA-01; exact buildings and incidental values are intentionally different.
- The game retains captions and explicit text evidence even where the concept relies more heavily on icons.

## Next bounded iteration

- Target: persistent HUD shell and activity-navigation density.
- Expected visual change: a single-line status bar with simulation controls, a compact icon-only activity rail with left-opening labels, and a footer limited to captions, destinations, and utilities.
- Acceptance observation: the default airport overview exposes materially more world, no locked destination or disabled current-destination button clutters the footer, and activity detail appears only after a deliberate activity-rail action.
