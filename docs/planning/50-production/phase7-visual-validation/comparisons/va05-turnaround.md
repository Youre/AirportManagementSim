# VA-05 Visual Comparison — Aircraft Turnaround

**Equivalent state:** First Cessna turnaround with automatic dispatch active  
**Board:** [actual versus concept](va05-turnaround.png)

## Review

- Selected aircraft identity, stand, exclusion zone, service tasks, automatic
  dispatch, task dependencies, readiness, schedule, and time controls are
  available in the live state.
- The first-use help card is an intentional Phase 7 layer and is dismissible;
  it does not replace the underlying turnaround controls.
- Progress and risk use text/icons alongside color, and the cursor remains
  available for hover, selection, and time control.

## Highest-impact differences

1. The concept depicts a larger regional jet with more service vehicles and
   simultaneous visible crews; the deterministic state is the approved
   starter Cessna.
2. The implementation world has less pavement material detail, lighting,
   animation, equipment variety, and human activity.
3. First-use guidance occupies more top-center space than the concept's small
   radio caption.

## Intentional differences

- The aircraft and tasks reflect the actual Phase 1 journey; using an
  unearned regional jet would make the comparison visually attractive but
  operationally false.
- The context card appears in this first-time state to meet the Phase 7
  guidance requirement and remains recoverable from Help after dismissal.

## Bounded correction completed

The context card was anchored to the upper gameplay region and capped at 45
words, leaving aircraft selection, task cards, readiness, and time controls
available while teaching the automatic-dispatch model once.
