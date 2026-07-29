# VA-03 Visual Comparison — Terminal Passenger Flow

**Equivalent state:** Operational domestic terminal with a selected passenger
party moving through departure and landside systems  
**Board:** [actual versus concept](va03-terminal-passenger-flow.png)

## Review

- The implementation retains the concept's labeled terminal cutaway, named
  Maya party, current step, route, time confidence, security state, baggage
  make-up/reclaim, accessible path, parking, curb, bus, and two-gate context.
- Green passenger, cyan secure, amber congestion, purple baggage/arrival, and
  dashed accessible routes are redundant with labels and the bottom legend.
- The world is substantially larger than either rail and remains selectable.

## Highest-impact differences

1. The concept has richer furniture, people, vehicles, storefronts,
   landscaping, and architectural shadows.
2. Route lines in the implementation are more diagrammatic and intersect more
   visibly around the compact terminal footprint.
3. The implementation's tool rail uses text-heavy operational controls rather
   than the concept's large asset palette.

## Intentional differences

- All routes and counts are query-backed. The implementation does not add
  decorative passengers or props that could be mistaken for simulation state.
- The selected party and actual RB 304 flow replace incidental concept values.

## Bounded correction completed

Terminal eligibility no longer permanently covers earlier views. Terminal is
an explicit destination with Back and Advanced actions, preserving both the
concept-mapped terminal state and the surrounding airport navigation.
