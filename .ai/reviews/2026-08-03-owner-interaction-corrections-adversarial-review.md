# Owner Interaction Corrections Adversarial Review

Date: 2026-08-03
Status: Approved with controls below

## Failure modes and controls

1. **Removing hover audio also removes useful feedback.** Keep the visual hover,
   focus, tooltip, click sound, invalid-action cue, and construction snap cue.
   Remove only the global mouse-enter sound.
2. **Taxiway body clicks still translate a segment.** Body hit testing must not
   return an editable segment. Only a displayed endpoint handle may enter the
   existing-segment edit path.
3. **Starting from a taxiway body creates a disconnected segment.** The first
   new point must pass through the same nearest-point snapping used by preview
   and topology validation.
4. **Endpoint editing is lost.** Tests must prove both endpoints remain
   selectable and movable while the segment center is not an edit hit.
5. **A body click exceeds the segment limit.** The existing maximum-segment
   guard and invalid-action feedback remain authoritative.
6. **The truck correction reverses workers or vehicle movement.** Apply the
   180-degree offset only to the construction truck visual. Do not change
   positions, interpolation, worker facing, or timing.
7. **Staff exposes a fake hiring mechanic.** The Phase 1 panel is read-only and
   explicitly explains automatic dispatch. It must not imply assignment controls
   that do not exist.
8. **Staff count conflicts with visible workers.** Use one fixture count of four
   for the completed team and the pre-team fallback, matching the four world
   construction-worker proxies.
9. **An old mid-construction save shows zero staff.** If no construction team
   exists yet, derive total staff from the fixture and assignment from stage.
10. **The panel permanently consumes the map.** Keep it closed by default,
    toggled from the icon rail, dismissible, responsive, and hidden during build
    mode. The normal world remains the dominant surface.
11. **Staff remains unavailable when it is useful.** Enable the rail action once
    Phase 1 is initialized, including while construction is underway.
12. **Compact layouts clip the panel.** Exercise the panel at the supported UI
    scales and keep its copy short enough for the compact rail/drawer contract.

## Required proof

Compilation, focused presentation/input/audio tests, the complete automation
suite, and a Windows packaged replay must prove the four corrections without
regressing taxi topology, pointer pan/zoom, construction, or Phase 1 behavior.
