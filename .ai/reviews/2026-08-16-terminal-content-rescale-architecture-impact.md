# Terminal content rescale architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

Make the doubled roofless terminal's contents readable at the same airport
overview where its 480x288-meter grown footprint is visible. Furniture must
occupy its authored footprint instead of reading as isolated dots; passengers,
staff, workers, bags, and service vehicles must remain identifiable without
obscuring room boundaries or circulation.

## Boundaries

- `AAMSimWorldPresenter` remains the only terminal and airport-world renderer.
- Terminal cells, object anchors, footprints, quarter turns, routes,
  construction jobs, persistence, and simulation checksums do not change.
- Static furniture sizing derives from the full sixteen-meter presentation
  cell and the existing 84-percent footprint-safe fill.
- Seat-group spacing uses the same full presentation cell so two-, four-, and
  six-seat compositions fill their authored multi-cell footprints.
- Mobile identity sprites use a separate twelve-times readability scale. Their
  anchor cells and gameplay occupancy remain unchanged.
- Floor, wall, door, roofless, sorting, camera, and pointer-deprojection
  contracts remain unchanged.

## Compatibility and rollback

This is presentation-only. It adds no save migration and no cooked assets.
Rollback is limited to shared presentation constants and their renderer uses.
Schemas 1-10 and every existing save remain compatible.
