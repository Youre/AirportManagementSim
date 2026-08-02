# Runway Surface and Construction Pacing Architecture Impact

Date: 2026-08-02
Status: Approved for implementation

## User-visible outcome

The starter-airfield build becomes readable work instead of a near-instant
transition. At 1x speed, material travel, site preparation, surface work, and
inspection occupy several real minutes; 2x through 8x remain the player's
explicit acceleration choices. Finished runway and taxiway surfaces read as
airport movement areas through dark compacted aggregate, runway edge/center/
threshold markings, and a yellow taxiway centerline. Reciprocal runway numbers
face the aircraft approaching from their respective runway ends.

## Simulation and persistence boundary

- Phase 1 remains authoritative for construction stage timing. The balance
  fixture changes from 0.5-minute increments to 30 minutes of travel, 30
  minutes of staging, 120 minutes of surface work, and 30 minutes of
  inspection: thresholds `1,800,000`, `3,600,000`, `10,800,000`, and
  `12,600,000` game milliseconds.
- The existing useful-road 20 percent reduction applies to the same thresholds.
- This is a balance-content change, not a save-schema change. Existing saves
  retain the funded timestamp and stage; after loading they reconcile against
  the current fixture exactly as other balance revisions do.
- Presentation progress continues to derive from immutable authoritative game
  time. No wall clock, actor tick, worker position, or texture state becomes
  gameplay authority.

## Asset and cooking boundary

- Local ComfyUI generates unmarked, strict top-down compacted aggregate
  candidates only. Deterministic source processing guarantees seamless edges,
  palette normalization, and precise project-authored runway/taxiway markings.
- Retained source masters and derivatives live under
  `SourceAssets/Phase1.5/Surfaces/`; provenance records generator workflow,
  prompt, dimensions, output checksums, processing, and review.
- New cooked assets use stable explicit names under
  `/Game/Phase1/Presentation/{Textures,Sprites}/Surfaces/` and constructor-time
  hard references. Runtime path-string loading remains prohibited.
- The legacy unmarked taxi-wear sprite remains the service-road and brown
  earthwork source. The marked taxiway sprite is never reused for a road or an
  unfinished surface.
- The editor commandlet gains a bounded `Phase1MovementSurfacesOnly` mode so
  importing these two surfaces does not rewrite unrelated presentation assets.

## Presentation boundary

- Existing arbitrary runway and taxiway geometry, proportional construction
  reveal, pooling, sorting, and simulation isolation remain unchanged.
- New final sprites replace only `RunwaySprite` and `TaxiSprite`; `AccessSprite`
  explicitly owns service-road and earthwork presentation.
- Runway-number yaw is centralized in the Phase 1 world-geometry helper. The
  primary end receives the reciprocal visual facing and the reciprocal end the
  primary visual facing, correcting the current 180-degree inversion.

## Verification and rollback

Focused tests must cover exact pacing thresholds, road reduction, midpoint
surface progress from fixture values, number yaw reciprocity, required asset
resolution, and correct road/earthwork sprite separation. Full automation,
editor/game builds, source/cook inventories, equivalent-state screenshots, and
a clean Development package are required because new runtime assets are added.

Rollback restores the four fixture values, the prior two sprite references,
and the former yaw expression. Saves remain schema compatible, and the new
assets can remain unused without affecting runtime state.
