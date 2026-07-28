# P6-04 Serious Incident Visual Comparison

- Reference: VA-06
- Equivalent state: protected aircraft-loss area with ordinary response
  resources and adjacent-runway continuity
- Implementation frame:
  [p6-04-serious-incident-continuity.png](../p6-04-serious-incident-continuity.png)
- Comparison board: [p6-04-serious-incident.png](p6-04-serious-incident.png)

## Validation

- [x] The affected area is localized rather than covering the airport.
- [x] Cause, consequence, remedy, and lifecycle are explicit.
- [x] Response route and ordinary vehicles are visible in the world.
- [x] The still-open runway is visually and textually confirmed.
- [x] Human outcome is abstract and non-graphic.

## Highest-impact differences

1. VA-06 centers a weather disruption; P6-04 uses the same response grammar
   for a serious but abstract aircraft-loss event.
2. The implementation makes continuing operations more prominent because
   Phase 6 explicitly tests adjacent-runway continuity.
3. The repair action is staged after investigation instead of offering an
   immediate generic recovery button.

## Bounded correction completed

The closure hatch was reduced to the affected runway segment and the open
runway received both a text card and uninterrupted world treatment. Acceptance
observation: a reviewer can identify what is closed, what remains open, and
the next legal recovery step.
