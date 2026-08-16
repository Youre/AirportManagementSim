# Terminal art orientation preliminary adversarial review

Date: 2026-08-16
Status: Approved with controls below

## Failure cases and controls

1. **Existing saves rotate semantically.** Do not mutate or migrate
   `QuarterTurns`; apply the correction only in the Paper2D presenter.
2. **The Rotate tool stops matching footprints.** Add the art-basis offset
   before the existing quarter-turn value so each player rotation remains a
   relative 90-degree step.
3. **Walls and doors become perpendicular to their edges.** Leave edge yaw
   derived exclusively from the edge vector.
4. **Square floors or overlays change unnecessarily.** Restrict the correction
   to `TerminalLayoutObjectProxies`.
5. **The current screenshot looks fixed but a future object regresses.** Route
   every placed-object kind through the shared correction and cover zero-turn
   plus one-turn presentation in automation.
6. **Upright is mistaken for strict nadir.** Record the remaining perspective
   mismatch and do not close the separate asset-remediation debt.
7. **Direct load opens an empty cutaway.** Prove a Phase 3 snapshot can create
   the terminal roof and cutaway without a preceding Phase 1 presentation
   call.

## Required proof

- Focused terminal-growth presentation automation.
- Editor/Game compilation.
- Packaged `VisualBaseline` terminal capture compared with the current
  pre-correction capture and VA-03.
- Canonical package refresh without clearing shared Unreal caches.
