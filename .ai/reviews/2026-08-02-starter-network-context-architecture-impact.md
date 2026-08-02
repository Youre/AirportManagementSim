# Starter Network Context Architecture Impact

Date: 2026-08-02
Status: Approved for implementation

## User-visible outcome

The starter map provides one small existing operations terminal with two
aircraft gate/stand connection ports. The player draws every runway and every
taxiway. A taxiway proposal is valid when its complete segment graph connects
at least one runway to at least one terminal gate and every proposed taxiway
segment belongs to that connected component. Roads are optional service paths;
they never require a stand connection and may reduce worker deployment time
when placed near construction.

## Affected contracts

- `FStarterPlanProposal` gains a bounded array of taxiway segments while
  retaining the legacy single-segment fields for schemas 1-8 and older code.
- Snapshots increment to schema 9. Schema-8 migration materializes the legacy
  taxi segment in the new array without changing completed-airport geometry.
- Phase 1 validation becomes graph-based. Segment adjacency includes shared
  endpoints, endpoint-to-segment joins, and true intersections.
- Fixed starter terminal and gate locations are map-authoritative helper data,
  not widget-owned gameplay state.
- The first connected gate becomes the Phase 1 assigned stand location for the
  one-flight slice. Later passenger-terminal interiors remain Phase 3 behavior.
- An optional road is represented by a non-zero road segment. It affects
  deterministic construction thresholds but not proposal validity.

## Presentation boundary

- C++ owns terminal/gate coordinates, taxi topology, snapping candidates,
  validation, migration, construction timing, and committed geometry.
- UMG owns terminal/gate depiction, small visual anchors, hover treatment, and
  concise diagnostics derived from the authoritative result.
- Paper2D uses a bounded pool for every retained taxi segment and renders the
  fixed starter terminal and both gate pads before construction.
- No new external or 3D asset is required; existing project-authored terminal,
  stand, surface, and marking sprites are reused through retained references.

## Compatibility and rollback

- Schemas 1-8 remain loadable. Future schemas remain rejected.
- Legacy committed layouts keep their stored stand, hut, road, and first taxi
  segment. New proposals use the fixed terminal/gate contract.
- Removing schema-9 fields and the new UI would restore schema-8 behavior, but
  schema-9 saves would then be intentionally rejected rather than misread.

## Verification impact

Add fixtures for chained and branched taxi segments, runway and gate reach,
disconnected segments, intersections, gate selection, road/no-road timing,
schema-8 migration, schema-9 round trip, pooled world geometry, snap targets,
and the packaged player journey.
