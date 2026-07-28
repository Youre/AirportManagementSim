# P6-05 Maximum-scale Visual Comparison

- Reference: VA-01 and VA-03
- Equivalent state: maximum-scale airport with aggregated logical population
  and bounded presentation pools
- Implementation frame:
  [p6-05-maximum-scale-overview.png](../p6-05-maximum-scale-overview.png)
- Comparison board: [p6-05-maximum-scale.png](p6-05-maximum-scale.png)

## Validation

- [x] World flow remains dominant at high population.
- [x] Logical and visible counts are distinguished.
- [x] Aggregation regions retain operational meaning.
- [x] Performance state is visible without developer-only jargon dominating.
- [x] Color-independent labels distinguish each population region.

## Highest-impact differences

1. VA-01 shows individually readable world objects; P6-05 deliberately
   aggregates most of 10,000 logical agents into bounded regions.
2. VA-03 focuses on terminal paths; Phase 6 includes terminal, concourse,
   cargo/service, and landside populations in one airport-wide surface.
3. The implementation exposes pool limits because visual density must remain
   bounded at the Phase 6 target.

## Bounded correction completed

Agent specks were reduced and grouped into four labeled operational regions,
while the exact contract counts moved into two central badges and the
inspector. Acceptance observation: scale reads as busy without hiding the
airport structure or overflowing the UI.
