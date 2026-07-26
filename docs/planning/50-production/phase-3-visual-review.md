# Phase 3 Visual Review

**Status:** Passed for the Phase 3 in-scope presentation
**Reviewed:** 2026-07-26
**Reference:** VA-03

## Evidence set

The deterministic complete-terminal fixture was captured at 1920 x 1080 and
100%, 125%, 150%, 175%, and 200% application scale. The primary equivalent
comparison is:

- `AMSim/Saved/Phase3/Comparisons/VA-03-Phase3.png`.

The reference is
`docs/planning/30-content-and-assets/concept-art/03-terminal-passenger-flow.png`;
the equivalent implementation state is
`AMSim/Saved/Phase1/Scale/100/va03-terminal-passenger-flow.png`. Generated
captures remain verification artifacts rather than runtime source assets.

## Details deliberately incorporated from VA-03

- deep-navy framing around a top-down terminal cutaway;
- persistent airport/economy/weather/time status;
- a compact terminal tool rail, large continuous world, right passenger rail,
  and bottom route/control legend;
- Gate A1/A2, departure lounge, check-in/bag drop, security, baggage make-up,
  arrivals, reclaim, entrance/curb, parking, and bus-stop labels;
- cyan controlled/departure treatment, purple arrival/reclaim treatment, amber
  baggage/congestion, green landside flow, and yellow accessibility;
- simultaneous directional passenger and baggage overlays;
- Maya's named four-person party, flight, current step/progress, needs, full
  route, accessible status, and time confidence;
- rounded panels/cards/buttons, cyan outlines, off-white type, concise
  operational copy, and non-color status text.

The implementation uses the real RB 304 fixture: 28 departing passengers,
24 arriving passengers, 34 checked bags, and exact authoritative
reconciliation.

## Iteration result

The first implementation matched the region structure but used one dark-teal
world language and allowed compact content to overrun at 175-200%. The bounded
iterations:

1. separated warm public floors, cool controlled rooms, purple arrivals,
   amber baggage, and green landside palette roles;
2. increased route weight and distributed passenger/bag proxies so concurrent
   populations remain visible rather than stacking on the same point;
3. replaced overflowing compact panels with bounded scrolling drawers and
   concise room labels;
4. reduced the high-scale legend while preserving every time/save/load action.

The recaptured 100%, 175%, and 200% states have no P0-P2 visual mismatch.

## Intentional differences

1. VA-03 contains bespoke illustrated furniture, plants, vehicles, and
   individual character art. Phase 3 uses reproducible project-owned Paper2D
   proxy assets and retains richer prop illustration as P3 art production.
2. The concept's construction palette remains open at all times. The
   implementation exposes only lifecycle-relevant typed actions, reducing
   clutter and preventing debug-style controls.
3. The concept includes rental cars and a denser curb scene. Rental and rail
   are Phase 4; Phase 3 deliberately shows private car, taxi, bus, parking, and
   curb only.
4. Concept values are illustrative. The implementation shows authoritative
   economy, flight, passenger, bag, security, and confidence values.

These differences do not alter VA-03's information hierarchy, route language,
or player task.

## Acceptance

The Phase 3 presentation passes because the concept's primary hierarchy and
semantic details are present, all commands are usable without debug surfaces,
the mouse/input policy is unchanged, state remains color-independent, the
complete scale matrix is readable, and the remaining asset-density difference
is isolated from simulation and reusable UI foundations.
