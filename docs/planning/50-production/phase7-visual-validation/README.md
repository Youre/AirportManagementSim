# Phase 7 Actual-Product Visual Validation

**Status:** Release-candidate visual audit complete; final owner checkpoint pending  
**Captured:** 2026-07-28  
**Resolution:** 1920 x 1080  
**Authority:** CT-05 written requirements, then the approved VA-01 through
VA-07 concept references

## Evidence boundary

Every image under `actual/` is an Unreal Engine capture produced by the
deterministic rendered fixtures. The side-by-side images under `comparisons/`
place those actual captures beside the approved concept references. No HTML,
Figma reconstruction, or substitute mock product is used as implementation
evidence.

Concept art is directional rather than pixel-authoritative. The audit requires
the same player purpose, information hierarchy, visual language, and essential
content. Simulation truth, accessible text, child-safe presentation, strict
top-down 2D, and player-operable controls override incidental generated-art
details.

## Result

| State | Actual implementation outcome | Review |
| --- | --- | --- |
| VA-01 airport overview | Mature mixed airport, persistent HUD, activity, selection, readiness, and transport evidence are present | [review](comparisons/va01-airport-overview.md) |
| VA-02 construction | Construction state, authoritative cost/status, affected site, work crew, and controls are present | [review](comparisons/va02-construction.md) |
| VA-03 terminal flow | Named party, complete passenger/bag/secure/accessible routes, exception state, and landside links are present | [review](comparisons/va03-terminal-passenger-flow.md) |
| VA-04 timetable | Seven-day schedule, contracts, feasibility, selected flight, resources, weather, and time controls are present | [review](comparisons/va04-regional-timetable.md) |
| VA-05 turnaround | Selected aircraft, automatic dispatch, parallel service state, dependencies, and operational controls are present | [review](comparisons/va05-turnaround.md) |
| VA-06 incident | Localized closure, continuing operations, weather, response progression, cause, remedy, and recovery action are present | [review](comparisons/va06-regional-incident.md) |
| VA-07 progression | Six equal, combinable paths and four explicit capability bands with evidence and objectives are present | [review](comparisons/va07-progression.md) |

The highest-impact audit correction was navigation ownership. Eligibility for a
later phase had been automatically replacing the screen under review. Terminal,
Regional, Advanced, and Major are now explicit destinations with visible Back
actions, so each concept-mapped surface remains directly inspectable.

## Scale and accessibility

The production Help/accessibility surface passes at 100%, 125%, 150%, 175%,
and 200%. Compact mode activates at 175-200% instead of shrinking text. See
[scale review](scale-review.md) and the actual captures under `scales/`.

## Remaining visual debt

- World art has lower prop, vegetation, lighting, and material density than the
  illustrative concepts.
- Some Phase 1 construction geometry is intentionally schematic while still
  carrying authoritative state.
- Aircraft/service sprite variety and close-operation animation remain below
  the concept's illustrative richness.
- The native text renderer has less typographic nuance than the painted
  concepts.

These are continuing polish items. None hides a required action, status,
resource, route, capability, or consequence. Final visual acceptance remains
part of the owner's consolidated post-Phase-7 test.
