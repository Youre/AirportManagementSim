# P6-02 Wide-body Turnaround Visual Comparison

- Reference: VA-05
- Equivalent state: selected Riverbend Longreach 787-9 at Stand H1 with
  ordinary-dispatch turnaround in progress
- Implementation frame:
  [p6-02-widebody-turnaround.png](../p6-02-widebody-turnaround.png)
- Comparison board:
  [p6-02-widebody-turnaround.png](p6-02-widebody-turnaround.png)

## Validation

- [x] The selected aircraft is the dominant object.
- [x] Service positions and approach path remain readable.
- [x] Passenger, bag, team, dependency, and safety state are concise.
- [x] One primary operation action is visible.
- [x] The approved River & Sun livery survives operational zoom.

## Highest-impact differences

1. VA-05 depicts a smaller aircraft and denser equipment ring; Phase 6 uses
   the true-scale 787 footprint and wider safety envelope.
2. The concept card stack names individual services; P6-02 summarizes the
   completed set and promotes only the next dependency.
3. Passenger and bag reconciliation is more explicit than the concept because
   it is a Phase 6 departure-readiness rule, not decorative information.

## Bounded correction completed

Equipment was moved outside the aircraft clearance envelope, the dependency
route was highlighted, and redundant task copy was collapsed. Acceptance
observation: the aircraft, current completion state, next blocker, and primary
action form a single scan path.
