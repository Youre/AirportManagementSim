# Construction Progress Feedback Adversarial Review

Date: 2026-08-02
Status: Approved with controls below

## Failure modes and controls

1. **The purchase succeeds but nothing appears.** Awaiting-delivery state must
   expose the full earthwork beds plus truck and crew at zero travel progress
   on the first post-command snapshot.
2. **Workers teleport to the site.** Their positions derive from normalized
   authoritative game time and interpolate from a visible origin to bounded
   targets on the accepted geometry.
3. **Pausing makes presentation disagree with simulation.** There is no actor
   tick or wall-clock animation. Paused simulation freezes travel and build
   progress; resume continues from the same state.
4. **A road is cosmetic or grants a mismatched visual speed.** The mapper uses
   the same `HasConstructionRoadBenefit` result and fixture percentage as the
   simulation thresholds.
5. **The final runway appears before it is built.** Awaiting-delivery shows no
   finished-surface overlay. Building reveals it by length. Inspection and
   later stages show the full approved surface.
6. **Taxi branches pop in or disappear.** Every bounded taxi segment receives
   the same normalized stage progress, so branches build proportionally;
   unused pool entries remain hidden.
7. **Runway numbers imply an operational surface too early.** Designators and
   windsock remain hidden until the completed surface reaches inspection.
8. **The accepted geometry changes during animation.** Partial endpoints are
   calculated into temporary presentation geometry only. The proposal and
   simulation checksum remain untouched.
9. **Generated dirt art introduces obvious seams or cooker risk.** Reviewed
   ComfyUI candidates are rejected unless they pass seam, zoom, palette,
   provenance, and cook checks. This pass retains no generated runtime asset.
10. **Proxy growth harms performance.** All construction components are a
    small fixed pool created in the actor constructor and refreshed only when
    the Phase 1 revision changes.
11. **Operational or cleared airports retain construction clutter.** Ready,
    operational, cancelled, and empty states must hide beds, crews, truck,
    cones, and protection zones as appropriate.
12. **Existing turnaround feedback regresses.** Construction refresh remains
    separate from fuel/inspection turnaround proxies and the prior turnaround
    count test stays intact.

## Required proof

Compile the editor/game targets, run focused construction and Paper2D tests,
run the complete automation suite, inspect accelerated delivery/build/
inspection captures against VA-02, and confirm source files remain within the
2,000-line standard. A package is required only if a new runtime asset or cook
dependency is retained; this implementation intentionally adds neither.
