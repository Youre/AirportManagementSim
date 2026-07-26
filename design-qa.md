# Phase 3 Design QA

**Status:** Passed
**Reviewed:** 2026-07-26
**Reference:** `docs/planning/30-content-and-assets/concept-art/03-terminal-passenger-flow.png`
**Equivalent capture:** `AMSim/Saved/Phase1/Scale/100/va03-terminal-passenger-flow.png`
**Comparison board:** `AMSim/Saved/Phase3/Comparisons/VA-03-Phase3.png`

## Equivalent state

The reference and implementation show the same player task: an open domestic
terminal with simultaneous arriving and departing passenger flow, an active
security queue, baggage routing, landside transport, Gate A1 service, and
Maya's selected party. The implementation uses the authoritative RB 304
fixture values rather than copying incidental concept-art numbers.

## Senior visual check

| Area | Result | Observation |
| --- | --- | --- |
| Typography | Pass | Off-white operational type, cyan section hierarchy, amber timing/economy emphasis, and short scanable labels are consistent. |
| Layout | Pass | Top status bar, compact left tools, continuous center world, right named-party inspector, and bottom flow/control legend match VA-03's region hierarchy. |
| Color and tokens | Pass | Warm landside/floor fields, cyan controlled areas, purple arrivals/reclaim, amber baggage/congestion, green landside, and yellow accessibility retain text support. |
| Assets | Pass with intentional P3 residual | Project-owned Paper2D room, route, passenger, bag, staff, vehicle, and aircraft proxies replace the concept's bespoke illustrative furniture and character art. |
| Copy | Pass | Labels use real Phase 3 state, concise cause/action language, authentic terms, and no debug-ledger text. |
| Interaction | Pass | Every Phase 3 mutation is player-facing; overlay cycling, time controls, save/load, scrolling drawers, and visible-mouse policy are preserved. |
| Responsive behavior | Pass | 100-150% retains the full rail composition. 175-200% uses bounded scroll drawers, concise world labels, and a compact control/legend strip without overlap. |

## Severity result

- P0: 0
- P1: 0
- P2: 0
- P3: one intentional future art-production opportunity: replace symbolic
  furniture/people density with a reviewed project-owned terminal prop set
  without changing the accepted layout, palette, or simulation contract.

## Bounded iterations completed

1. The initial terminal capture was structurally correct but visually too
   dark and sparse. Room fields were separated into warm, secure, arrivals,
   baggage, and landside palette roles; passenger and bag proxy placement was
   de-overlapped; route weight was increased.
2. The first 175-200% captures allowed content to escape the compact panels.
   Both rails now use clipped scroll drawers, world labels shorten without
   losing meaning, and the compact legend/control strip fits the viewport.

No unresolved design defect blocks Phase 3.
