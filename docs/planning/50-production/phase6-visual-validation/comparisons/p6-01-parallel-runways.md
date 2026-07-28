# P6-01 Parallel Runway Visual Comparison

- Reference: VA-01 and VA-04
- Equivalent state: Major airport operating independent arrival and departure
  runways
- Implementation frame:
  [p6-01-parallel-runway-operations.png](../p6-01-parallel-runway-operations.png)
- Comparison board: [p6-01-parallel-runways.png](p6-01-parallel-runways.png)

## Validation

- [x] World remains the dominant continuous region.
- [x] Reciprocal and parallel designations are explicit and correct.
- [x] Configuration, capacity, and crossing protection are visible together.
- [x] Primary and secondary actions have distinct hierarchy.
- [x] Status uses labels, borders, and dots in addition to color.

## Highest-impact differences

1. VA-01 shows a mature mixed airport with more decorative world detail; P6-01
   deliberately isolates the runway network so configuration remains legible.
2. VA-04 is timetable-first; P6-01 moves timetable impact into a secondary
   action because runway configuration is the selected context.
3. The concept uses softer surface lighting; the implementation retains flat,
   high-contrast Paper2D surfaces for reliable zoom readability.

## Bounded correction completed

The runway labels, reciprocal side swap, independent-use text, and crossing
window were all brought into one surface. Acceptance observation: a reviewer
can identify `09L/27R`, `09R/27L`, their roles, and the protected next action
without opening another panel.
