# VA-01 Visual Comparison

- Reference: C:\Users\dave\Documents\AirportManagementSim\docs\planning\30-content-and-assets\concept-art\01-airport-overview-hud.png
- Capture: C:\Users\dave\Documents\AirportManagementSim\AMSim\Saved\VisualReview\compact-hud-normal.png
- Comparison board: C:\Users\dave\Documents\AirportManagementSim\AMSim\Saved\VisualReview\VA-01-compact-hud.png
- Equivalent state: Operational airport overview with compact map navigation

## Review checklist

- [x] World versus UI dominance matches the reference intent.
- [x] Primary, secondary, and contextual actions have a clear hierarchy.
- [x] Palette, surface treatment, outlines, and depth cues form one visual language.
- [x] Typography size, weight, casing, and spacing are coherent.
- [x] Panel density and whitespace support quick scanning.
- [x] Operational state remains legible without relying on color alone.
- [x] Mouse cursor remains visible and interactive in the gameplay viewport.

## Highest-impact mismatches

1. The concept uses richly illustrated, individually labeled tools; the Phase 1
   implementation reuses the simpler project icon atlas and reveals labels on
   hover or keyboard focus.
2. The concept airport is mature and visually dense, while the equivalent
   implementation state is intentionally a sparse starter airfield under
   construction.
3. The implementation retains a taller objective rail and a broad development
   control strip, so its peripheral UI remains more utilitarian than the
   concept's aircraft-focused lower inspector.

## Intentional differences

- Collapsed labels are intentional: this user-requested density reduction keeps
  the world fixed while the rounded label flyout overlays it temporarily.
- The starter state cannot reproduce the mature facility and traffic density of
  VA-01 without inventing later-phase gameplay content.

## Next bounded iteration

- Target: compact left navigation and map-dominant overview.
- Expected visual change: icon-only resting rail with stable rounded label
  flyouts and no persistent text-column cost.
- Acceptance observation: accepted for this bounded pass. The 112-unit rail,
  88 by 56 icon targets, and overlay flyout preserve the map layout; richer
  icon illustration and the remaining root-HUD density stay as visual debt.
