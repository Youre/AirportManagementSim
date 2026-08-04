# Owner Interaction Corrections Architecture Impact

Date: 2026-08-03
Status: Approved for implementation

## User-visible outcome

- Ordinary button hover remains visually responsive but silent. Click,
  validation, snapping, and other semantic UI cues remain audible.
- With the taxiway tool active, pressing an existing taxiway body starts a new
  connected segment. Only an endpoint handle edits an existing segment.
- The construction truck faces the direction implied by its source artwork.
- The Staff rail action opens a concise Phase 1 panel that shows the four-person
  construction crew, current availability, assignment, and automatic-dispatch
  status.

## Affected contracts

- The global Riverbend button style no longer assigns a hovered Slate sound.
  No sound asset, cook rule, or persisted setting changes.
- Taxiway pointer hit testing distinguishes editable endpoints from segment
  bodies. Segment-body presses reuse the existing new-segment and snap path;
  proposal geometry and schema 9 remain unchanged.
- The truck receives a presentation-only 180-degree sprite-axis correction.
  Simulation positions and construction timing remain unchanged.
- `FPhase1Fixture` declares the four-worker starter construction crew. The Staff
  presenter derives its display from immutable Phase 1 state, falling back to
  that fixture value for old or in-progress state without a team record.

## Presentation boundary

- C++ remains authoritative for construction stage, team size, geometry,
  snapping, and tool selection.
- A focused Phase 1 Staff presenter maps authoritative state into concise text;
  its UMG view cannot dispatch, hire, or mutate simulation state.
- The Staff panel is a temporary map overlay opened from the existing icon rail
  and closes automatically when build mode takes over the map.
- No new asset, runtime string load, Blueprint mutation, schema revision, or
  cooking dependency is introduced.

## Compatibility and rollback

- Existing saves remain compatible because all changed data uses existing
  fields. Older saves without a construction-team record use the fixture count.
- Reverting the focused changes restores the prior gestures and placeholder
  Staff action without a migration.

## Verification impact

Add focused coverage for silent hover styles, preserved click sounds,
endpoint-only taxiway editing, the corrected truck yaw, four-worker state
mapping across construction stages, and the enabled Staff rail journey. Then
run the complete automation suite and replay the packaged build-mode journey.
