# Unified build-world architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

- Build mode and normal play show the same airport, terminal, gates, terrain,
  camera, scale, and coordinates.
- Runway, taxiway, and service-road gestures preview directly in the Paper2D
  world. Confirming a valid proposal changes construction state and treatment;
  it does not replace the preview with differently positioned artwork.
- Panning and zooming cannot make the pointer, snap targets, or preview drift
  away from the world.

## Existing debt being retired

- `UAMSimConstructionProposalView` currently owns a second airport renderer:
  normalized UMG boxes for the runway, taxiways, road, terminal, and gates.
- That renderer hard-references the superseded regional-terminal texture and
  duplicates the parcel-to-screen transform, dimensions, rotation, tint, and
  visibility rules already owned by `AAMSimWorldPresenter`.
- Opening build mode currently hides the authoritative Phase 1 world network
  and fixed context, which permits the two render paths to disagree.

## Target boundary

- `AAMSimWorldPresenter` is the only owner of airport-world rendering. It owns
  committed infrastructure, generated schema-10 terminal presentation, and a
  presentation-only pool for uncommitted construction previews.
- `AMSimPhase1WorldGeometry` is the single conversion boundary between Phase 1
  parcel coordinates and world coordinates in both directions.
- `UAMSimConstructionProposalView` remains responsible for tools, validation,
  instructions, undo, confirmation, and pointer gestures. It deprojects the
  pointer onto the world and sends immutable preview state to the presenter.
- UMG remains screen-space chrome. It must not render terminals, gates,
  movement surfaces, or other airport-world substitutes.
- Phase 1 simulation remains authoritative for proposal validation, credits,
  commitment, construction stages, and saves. No schema or gameplay rule
  changes are required.

## Organization

- Put construction-preview proxy behavior in a focused world-presenter source
  file instead of extending the 1,982-line presenter constructor.
- Remove obsolete artwork properties and world-geometry widget state from the
  build view. Keep its existing tool and gesture logic behind a narrow preview
  synchronization method.
- Do not modify or stage the unrelated untracked `docs/3d_workflows/` content.

## Compatibility and rollback

- Schemas 1-10 are unchanged. Existing proposal and construction records render
  through the same committed path as before.
- A rollback can remove the preview pool and restore the prior UMG preview
  without changing saved state. No migration or asset rewrite is involved.

## Required proof

- Automated coverage must prove invertible world/parcel mapping, preview and
  committed segment geometry parity, preview cleanup, generated-terminal
  continuity, and the absence of the legacy construction-view artwork load.
- A packaged visual replay must compare the same proposal before and after
  confirmation and verify camera pan/zoom, selection, snapping, cancellation,
  and terminal/gate continuity.
