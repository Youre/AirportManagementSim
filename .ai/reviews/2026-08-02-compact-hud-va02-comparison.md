# VA-02 Visual Comparison

- Reference: C:\Users\dave\Documents\AirportManagementSim\docs\planning\30-content-and-assets\concept-art\02-build-mode-runway-extension.png
- Capture: C:\Users\dave\Documents\AirportManagementSim\AMSim\Saved\VisualReview\compact-hud-normal.png
- Comparison board: C:\Users\dave\Documents\AirportManagementSim\AMSim\Saved\VisualReview\VA-02-compact-construction-card.png
- Equivalent state: Confirmed runway construction with concise active-work status

## Review checklist

- [x] World versus UI dominance matches the reference intent.
- [x] Primary, secondary, and contextual actions have a clear hierarchy.
- [x] Palette, surface treatment, outlines, and depth cues form one visual language.
- [x] Typography size, weight, casing, and spacing are coherent.
- [x] Panel density and whitespace support quick scanning.
- [x] Operational state remains legible without relying on color alone.
- [x] Mouse cursor remains visible and interactive in the gameplay viewport.

## Highest-impact mismatches

1. VA-02 depicts a pre-commit runway-extension editor with validation, cost,
   conflict, and confirmation surfaces; the capture depicts the deliberately
   quieter post-commit construction state.
2. The reference has stronger proposal/closure patterning and denser contextual
   controls; the implementation currently concentrates its active state in
   workers, surface reveal, endpoint markers, and one compact card.
3. The concept's mature airport, construction props, and environment are more
   richly illustrated than this starter-airfield world.

## Intentional differences

- The compact card intentionally omits the repeated `STARTER AIRFIELD`, taxi
  network, and gate copy. It preserves only stage, runway identity, and reserved
  cost; detailed controls belong to the project/build surface.
- This is an active-project comparison, not an assertion that the current
  capture reproduces the concept's proposal state.

## Next bounded iteration

- Target: concise active-construction status at the map corner.
- Expected visual change: one short rounded card that reads in two lines and
  leaves the work site unobscured.
- Acceptance observation: accepted for this bounded pass. The card is concise,
  status remains text-encoded, and the world stays dominant; richer construction
  props and proposal-pattern fidelity remain continuing visual debt.
