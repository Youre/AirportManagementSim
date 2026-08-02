# VA-02 Visual Comparison

- Reference: C:\Users\dave\Documents\AirportManagementSim\docs\planning\30-content-and-assets\concept-art\02-build-mode-runway-extension.png
- Capture: C:\Users\dave\Documents\AirportManagementSim\AMSim\Saved\Phase1\va02-construction.png
- Comparison board: C:\Users\dave\Documents\AirportManagementSim\AMSim\Saved\VisualReview\VA-02-construction-patrol-facing.png
- Equivalent state: Starter airfield building at 1920x1080, 100% UI scale, reference profile zoom

## Review checklist

- [x] World versus UI dominance matches the reference intent: both reserve the
  center for a large, continuously readable airfield view.
- [x] Primary, secondary, and contextual actions have a clear hierarchy for the
  captured active-construction state.
- [x] Palette, surface treatment, outlines, and depth cues form one visual
  language within the current Riverbend component and world kit.
- [x] Typography size, weight, casing, and spacing are coherent.
- [x] Panel density and whitespace support quick scanning.
- [x] Operational state remains legible without relying on color alone: the
  status card explicitly says `SURFACE WORK ACTIVE` and describes the crew.
- [ ] Mouse cursor visibility is not observable in the unattended offscreen
  capture; the existing interactive cursor acceptance remains unchanged.

## Highest-impact mismatches

1. VA-02 depicts a mature, richly populated airport with buildings, vehicles,
   trees, apron detail, and layered roads; the starter implementation remains a
   sparse first-airfield parcel, so its world lacks comparable visual density.
2. The concept gives planning conflicts and proposed geometry strong red/blue
   patterned overlays. This active-building capture prioritizes the brown-to-
   finished surface transition and small cones, making the protected work area
   less visually prominent.
3. The concept uses larger, more numerous worker and vehicle clusters around
   the worksite. The implementation's four moving workers and delivery truck
   are correctly distributed and oriented, but their small silhouettes provide
   less construction activity and direction readability at this zoom.

## Intentional differences

- VA-02 is a runway-extension planning state at a mature airport; this capture
  is the Phase 1 starter airfield after commitment, during surface work.
- The runtime shows only player-authored runway/taxiway geometry plus the fixed
  basic terminal context; mature facilities and scenery are not fabricated for
  a Phase 1 state.
- Written gameplay requirements keep four presentation-only workers and one
  delivery truck; they do not imply additional simulation entities.

## Next bounded iteration

- Target: Construction-worker and delivery-vehicle motion/facing.
- Expected visual change: Short staggered runway/taxiway/road patrols replace
  the nearly static full-stage sweep, and every moving construction sprite
  rotates to face its actual route direction.
- Acceptance observation: Accepted for this bounded iteration. The capture
  shows separated horizontal runway crews and a vertical taxiway crew with
  matching orientation; focused automation proves position changes, reversal,
  loop continuity, pause freezing, and the four cardinal facing directions.
  Richer work-zone activity, sprite scale, and environmental density remain
  continuing visual-polish debt rather than regressions in this change.
