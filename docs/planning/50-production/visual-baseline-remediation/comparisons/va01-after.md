# VA-01 Visual Comparison

- Reference: C:\Users\dave\Documents\AirportManagementSim\docs\planning\30-content-and-assets\concept-art\01-airport-overview-hud.png
- Capture: C:\Users\dave\Documents\AirportManagementSim\docs\planning\50-production\visual-baseline-remediation\audit\17-packaged-final-visual-baseline-overview.png
- Comparison board: C:\Users\dave\Documents\AirportManagementSim\docs\planning\50-production\visual-baseline-remediation\comparisons\va01-after.png
- Equivalent state: Established airport overview with active operations, schedule pressure, staffed services, and a selected aircraft

## Review checklist

- [x] World versus UI dominance matches the reference intent.
- [ ] Primary, secondary, and contextual actions have a clear hierarchy.
- [x] Palette, surface treatment, outlines, and depth cues form one visual language.
- [x] Typography size, weight, casing, and spacing are coherent.
- [x] Panel density and whitespace support quick scanning.
- [x] Operational state remains legible without relying on color alone.
- [x] Mouse cursor remains visible and interactive in the gameplay viewport.

## Highest-impact mismatches

1. The selected aircraft still has no VA-01-style bottom inspector for identity,
   readiness, service progress, and contextual actions. The footer remains a
   utility/destination row rather than the selected-entity workspace.
2. The airport occupies a compact central island with broad unused grass,
   whereas VA-01 uses runway, apron, terminal, roads, landscape, and water to
   create a denser site-wide composition.
3. Alerts, Flights, and Projects now use the correct compact right-rail pattern,
   but the icons lack badges and still open one general-purpose operations
   drawer instead of category-specific, concise panels.

## Intentional differences

- The fixture is a flight-school/regional airport rather than VA-01's mixed GA
  airport; buildings, aircraft, economy values, and weather are state-derived.
- Captions and save/load/window utilities remain explicit because they are
  functional and accessibility requirements, even though the concept minimizes
  them.

## Next bounded iteration

- Target: root selected-aircraft inspector and action hierarchy.
- Expected visual change: selecting an aircraft replaces the utility-heavy
  bottom region with a bounded identity/readiness/service/action inspector that
  reuses real Phase 1-4 state and preserves captions and utilities in a compact
  secondary layer.
- Acceptance observation: the selected aircraft can be understood and acted on
  from the root overview without opening a diagnostic screen, while the airport
  retains at least the current world area.
