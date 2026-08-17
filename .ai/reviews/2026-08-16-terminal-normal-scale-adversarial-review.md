# Terminal normal-scale adversarial review

Date: 2026-08-16
Status: Passed with packaged visual inspection required

## Failure modes reviewed

1. **A larger terminal becomes a second view.** Rejected: camera behavior and
   tool navigation are unchanged; only world geometry grows at its fixed
   anchor.
2. **Mouse placement targets the former one-meter rendering.** Prevented by a
   shared cell-world-unit contract used by both rendering and deprojection.
3. **Furniture becomes four times too large.** Prevented by separating the
   facility footprint multiplier from the content-readability multiplier.
4. **Opening Terminal changes scale or position.** Guarded by existing
   same-frame tests plus explicit footprint equality before and after tools.
5. **The larger footprint collides with roads or facilities.** Requires
   equivalent-state packaged inspection against `VisualBaseline`; adjust the
   bounded terminal campus, not the camera or renderer, if a conflict appears.
6. **Old saves or deterministic simulation change.** No serialized state or
   simulation logic changes. Full automation and save-preserving canonical
   refresh remain required.
7. **Roof or legacy terminal reappears.** Existing zero-roof and legacy-proxy
   assertions remain mandatory.
8. **Scale copy lies to the player.** Gesture feedback reports the presented
   eight-meter module dimensions instead of raw logical-cell counts.

## Gate

Do not call the change complete until the packaged mature airport visibly
contains a readable roofless terminal at overview scale, pointer mapping remains
aligned, and the one-camera Terminal-tools replay passes.

## Resolution

The first eight-meter packaged capture did reveal an access-road overlap. A
single presentation-only 60-meter landside-campus offset corrected it. The
recaptured terminal clears the road, stays within the world viewport, and does
not change player camera framing. All other reviewed failure modes remain
covered by focused and complete automation.
