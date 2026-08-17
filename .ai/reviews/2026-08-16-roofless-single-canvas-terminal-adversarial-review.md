# Roofless single-canvas terminal preliminary adversarial review

Date: 2026-08-16
Status: Approved with controls below

## Failure cases and controls

1. **The roof disappears but the interior remains conditional.** Floor,
   edges, furniture, workers, visitors, and spatial passenger proxies must be
   visible whenever a non-empty schema-10 layout exists.
2. **Opening Terminal still changes framing indirectly.** Remove terminal
   focus/restore state from the camera and assert identical location and ortho
   width across panel open and close.
3. **The player cannot zoom close enough without terminal mode.** Use one
   adaptive global zoom curve with a close-inspection minimum and airport-wide
   maximum.
4. **Close zoom becomes jumpy.** Scale wheel increments from current ortho
   width rather than using the former 5,000-unit fixed step.
5. **Roof details leak above furnishings.** Keep roof surface, edge, corner,
   canopy, and HVAC proxies hidden in every mode.
6. **Legacy Phase 3 terminal art returns outside the tool panel.** Keep all
   fixed-coordinate legacy terminal surfaces and routes hidden whenever the
   schema-10 terminal is authoritative.
7. **Terminal tools affect unrelated world visibility.** Interaction mode may
   enable placement previews only; it must not suppress or restore mature
   facilities, infrastructure, aircraft, people, incidents, or landscaping.
8. **Always-visible people overwhelm the airport overview.** Preserve current
   world scale and pooling. Any future zoom-based simplification must change
   detail only, never position, camera, or authoritative entity ownership.
9. **Save compatibility regresses.** Do not change serialized state, schema,
   IDs, migrations, or layout dimensions.
10. **Documentation continues describing roof/cutaway swapping.** Update the
    dynamic state and active terminal plan to make roofless single-canvas
    presentation authoritative.

## Required proof

- Focused presenter and camera automation followed by the complete AMSim suite.
- Same-frame packaged captures before and after opening Terminal tools.
- Clean Development and Shipping packages, runtime smoke, release audit, and
  zero forbidden editor/MCP/runtime-string-load matches.
