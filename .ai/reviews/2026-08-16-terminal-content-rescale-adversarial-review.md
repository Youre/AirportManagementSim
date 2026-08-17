# Terminal content rescale adversarial review

Date: 2026-08-16
Status: Passed with packaged visual gate required

## Failure modes

1. **Furniture remains tiny because only actor sprites change.** Static object
   fit and seat spacing must use the sixteen-meter presentation cell directly.
2. **Furniture overhangs rooms or walls.** Uniform aspect-preserving fit stays
   bounded to 84 percent of the authored rotated footprint.
3. **Seat groups collide or stop representing their saved count.** Preserve the
   exact 2/4/6 proxy count and keep pattern extents within the saved multi-cell
   footprint.
4. **People or bags become icons larger than furniture.** Mobile sprites use a
   separate twelve-times readability token rather than the full cell size.
5. **Vehicles cover the terminal interior or access road.** Their anchors and
   count remain fixed; the packaged overview must reject visible obstruction.
6. **Presentation size changes gameplay occupancy.** No simulation, route,
   command, save, or serialized field is modified.
7. **Large contents render below floors or above screen-space chrome.** Existing
   world sort priorities remain unchanged and are inspected in the packaged
   equivalent state.
8. **The correction hides the terminal/camera debt by changing zoom.** Camera
   code is out of scope; comparison uses the identical 1920x1080 Phase 4 state.

## Gate

Accept only an equivalent-state packaged capture where furniture and moving
entities are visibly legible inside the entire grown terminal, stay within
their room/route context, preserve exact seat counts, and do not alter the
single-canvas composition.
