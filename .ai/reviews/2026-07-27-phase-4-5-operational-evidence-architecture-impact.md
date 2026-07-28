# Phase 4.5 Operational Evidence Architecture Impact

**Status:** Passed  
**Date:** 2026-07-27  
**Scope:** starter construction lifecycle, turnaround safety choreography,
regional contract/risk evidence, and specialization inspection

## Affected ownership

- `AMSimUI` owns every change in this pass: pooled Paper2D proxies, immutable
  query-to-widget mapping, state labels, and visual proof accessors.
- `AMSimSimulation`, command/event contracts, schema 5, deterministic fixture
  values, saves, and economy behavior remain unchanged.
- `AMSimGameplay` may only settle and capture already-authoritative states; it
  must not synthesize state for presentation.

## Interface and data-flow impact

- Phase 1 world presentation gains bounded project-owned proxies for delivery,
  construction, inspection, safe vehicle staging, and automatic turnaround
  dispatch.
- Proxy visibility is derived from `FPhase1QuerySnapshot`; no presentation
  position or selection is written back to the simulation.
- Regional contract, flight-risk, and progression inspection continue to read
  existing Phase 4 and cross-phase snapshots.
- All new visible art references resolve through constructor-time PaperSprite
  references already included in the Phase 4.5 asset kit.

## Preserved invariants

- Unreal remains 2D-first; no 3D assets or online runtime dependencies are
  introduced.
- Cancellation clears committed construction proxies because the authoritative
  construction stage returns to `None`; refund behavior remains simulation-owned.
- Turnaround vehicles remain automatically dispatched. The UI may communicate
  priority and safe assignment but does not expose manual driving.
- Conflict and risk treatments pair color with pattern, text, or authored
  spatial markers.

## Source organization and rollback

- Focused Phase 1 operations presentation belongs in a dedicated
  `AMSimWorldPresenterPhase1Operations.cpp` translation unit.
- The core presenter remains below the 2,000-line project limit.
- Rollback removes the focused proxies/accessors and restores the prior scene
  without touching simulation state or saved data.

## Required evidence

- Unreal 5.8.0 editor build.
- `AMSim.Phase4_5` and Phase 1 Paper2D presentation automation.
- Phase 1 rendered construction/turnaround smoke captures.
- Equivalent-state VA-02, VA-04, VA-05, and VA-07 comparison boards.

