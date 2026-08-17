# Terminal furnishing consistency preliminary adversarial review

Date: 2026-08-16
Status: Approved with controls below

## Failure cases and controls

1. **One sprite is stretched into several seat counts.** Compose one proxy per
   seat and assert that the starter layout renders the sum implied by every
   `SeatGroup2`, `SeatGroup4`, and `SeatGroup6` record.
2. **A generated candidate repeats the old perspective error.** Retain only the
   strict-nadir single-seat candidate; reject the pseudo-perspective and
   background-bearing group candidates.
3. **Furniture exceeds its footprint.** Fit every non-seat sprite uniformly to
   the rotated footprint with bounded padding; never use independent X/Z
   scales.
4. **Quarter turns rotate art but not the seat pattern.** Rotate both the
   modular-seat offsets and each seat's facing around the authoritative group
   center.
5. **Back-to-back seating all faces one direction.** Give the second row a
   relative 180-degree facing while preserving the player's group rotation.
6. **Proxy counts grow on refresh.** Reuse the seat pool and hide unused
   components after each revision-gated refresh.
7. **The asset works in Editor but disappears in Shipping.** Import it through
   the existing presentation commandlet, use a constructor-time reference,
   retain `/Game/TerminalGrowth` cook coverage, and inspect packaged output.
8. **New art silently changes gameplay.** Do not modify terminal state,
   readiness, capacity, pathfinding, construction, or persistence.

## Required proof

- Focused `AMSim.TerminalGrowth.Presentation` automation.
- Editor and Game compilation.
- Development and Shipping package smoke with the canonical package refreshed.
- Equivalent-state visual board against VA-03 and the pre-change packaged
  capture.
