# Terminal furnishing consistency architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

Terminal seating reads as real two-, four-, and six-seat furniture in strict
top-down view. Furniture fits its simulation footprint without image stretching,
and quarter-turn rotation remains predictable.

## Affected boundaries

- Presentation only: `AAMSimWorldPresenter` composes seat groups from one
  cooker-visible, project-owned single-seat PaperSprite.
- The Phase 3 terminal layout remains authoritative for object kind, anchor,
  footprint, clearance, construction state, and quarter turns.
- The simulation schema, save format, economy, construction, routing, and
  readiness contracts do not change.
- Non-seat furnishings keep one pooled proxy per placed object, but use a
  uniform aspect-preserving fit inside their logical footprint.
- Seat groups use a separate pooled proxy collection, one proxy per visible
  seat, so exact seat counts remain legible without pre-baked stretched atlases.

## Asset and cooking impact

- Add one OpenAI-authored strict-nadir seat master under `SourceAssets` and one
  normalized 256-by-256 runtime texture.
- Add a constructor-time reference and presentation-commandlet import for
  `/Game/TerminalGrowth/Presentation/Sprites/Furniture/S_SeatSingle`.
- `/Game/TerminalGrowth` remains explicitly cooked. Runtime string loading is
  not introduced.

## Compatibility and rollback

Schemas 1-10 remain byte-compatible. Rollback removes the modular seat proxy
pool and restores the previous object-sprite mapping; no save migration is
required.

## Required proof

- Terminal presentation automation covers exact composed seat count, pooling,
  uniform scale, direct restore, and quarter turns.
- Development and Shipping packaging cook and exercise the new sprite.
- A current packaged terminal capture is compared with VA-03 and the pre-change
  furnishing capture.
