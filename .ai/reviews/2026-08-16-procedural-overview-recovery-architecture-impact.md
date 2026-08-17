# Procedural overview recovery architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

Loading a mature save returns to one coherent airport overview. The overview
shows the exterior airport and never exposes the retired fixed-coordinate
Phase 3 terminal proof renderer. The Terminal action remains the only entry to
the authoritative schema-10 cutaway.

Large overview facilities use strict-nadir procedural footprints instead of
independently stretched building and landside sprites. Small people, vehicles,
aircraft, fixtures, and reviewed landscaping remain sprite-based where their
silhouette carries useful identity.

## Affected boundaries

- `UAMSimTerminalView::ClosePresentation` restores the airport-overview mode
  explicitly. Save data remains simulation-only and does not persist a UI
  destination.
- `AAMSimWorldPresenter` remains the sole airport-world renderer. It owns the
  procedural facility meshes beside the existing procedural runway, taxiway,
  apron, gate, and road meshes.
- `AMSimProceduralSurfaceGeometry` remains presentation-only. The new facility
  meshes are generated from authored overview definitions and never write
  their locations back into simulation.
- The schema-10 terminal layout remains the only supported terminal interior
  representation. The older Phase 3 fixed-coordinate arrays stay isolated for
  legacy proof surfaces and are hidden on the root overview.
- No simulation, save schema, economy, pathfinding, scheduling, or progression
  contracts change.

## Visual contract

- VA-01 owns the airport-overview comparison: orthographic world first, deep
  navy chrome, readable exterior zones, restrained operational density.
- VA-03 owns the Terminal cutaway comparison: spatial rooms, furnishings,
  passengers, baggage, and route overlays appear only at terminal scale.
- Procedural facility geometry uses world-layer heights and sort priorities so
  roads remain below structures and markings/details remain above their base.
- Geometry is deterministic, resolution-independent, and cooker-safe through
  the existing constructor-visible procedural material.

## Compatibility and rollback

Schemas 1-10 remain byte-compatible. Rollback removes the facility mesh pool
and restores the mature-site sprite visibility rules; no save migration is
required.

## Required proof

- Automation verifies overview mode hides all legacy terminal-interior proxies,
  shows the generated terminal roof, and activates valid procedural facility
  meshes.
- The load, Terminal, and Back journeys are captured separately in a packaged
  Development build.
- Full automation, clean Development and Shipping packages, cooked-asset and
  forbidden-dependency audits, and a canonical owner-build refresh pass.
