# Terminal double-scale architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

Double the terminal's linear airport-world footprint from the rejected
eight-meter presentation module to a sixteen-meter module. The 18x12 starter
layout will present at approximately 288x192 meters and the 30x18 grown layout
at 480x288 meters. The roofless terminal must become a major central facility
at airport overview zoom rather than a small plan tile at the bottom edge.

## Boundaries

- `AAMSimWorldPresenter` remains the only airport and terminal world renderer.
- No world switch, terminal camera, overview zoom adjustment, or widget-local
  facility renderer is introduced.
- Schema-10 cells, stable IDs, routes, costs, construction, persistence, and
  deterministic simulation state remain unchanged.
- Rendering, placement previews, pointer deprojection, and physical-size copy
  continue to consume one shared presentation geometry contract.
- The fixed presentation campus moves between the apron and landside access
  road so the doubled footprint stays in the useful world composition. Airside
  overlap with the apron is deliberate; road overlap is not.
- Identity-bearing furnishings and moving entities double with the building so
  their relative screen-space readability is preserved.

## Compatibility and rollback

This remains presentation-only and requires no save migration or new cooked
asset. Rollback is limited to the shared module, content scale, and campus
offset constants. Existing schema 1-10 saves remain compatible.
