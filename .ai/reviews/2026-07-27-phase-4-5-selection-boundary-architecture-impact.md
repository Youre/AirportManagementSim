# Phase 4.5 Selection, Boundary, and Weather Architecture Impact

**Status:** Passed  
**Date:** 2026-07-27  
**Scope:** VA-01 selected entities, VA-03 controlled/baggage world evidence,
and VA-06 wet-surface depth

## Affected ownership

- `AMSimUI` owns the new transient overview selection mode, immutable
  query-to-inspector mapping, Paper2D focus target, terminal boundary/exception
  proxies, passenger identity art, and incident wet layers.
- `AMSimGameplay` changes only the development smoke capture schedule so the
  final proof waits for the selected presentation state to settle.
- `AMSimSimulation`, schema 5, command/event contracts, fixture values, and
  save data are unchanged.

## Interfaces

- `UAMSimOverviewView` exposes transient aircraft/facility selection and
  delegates the matching world focus request.
- `AAMSimWorldPresenter` exposes a presentation-only selection target and
  bounded proxy-count evidence for tests.
- All identity and world assets remain constructor-time or serialized UObject
  references; no runtime string lookup was added.

## Preserved invariants

- Selection coordinates never feed simulation state.
- Exception visuals derive from immutable bag state and render a clear branch
  when no exception is active.
- Wet layers are duplicated project-owned Paper2D surface assets below route,
  closure, aircraft, and response overlays.
- No 3D content, save migration, online dependency, or Shipping tool
  dependency is introduced.

## Source organization and rollback

- Focused terminal, incident, and visibility presenter files contain the new
  composition logic.
- All touched UI source files remain below the 2,000-line limit; the largest is
  `AMSimRegionalOperationsView.cpp` at 1,956 lines.
- Rollback removes the transient delegates/proxies and restores the prior
  screenshot stage without affecting saved games or simulation checksums.

## Required evidence

- Unreal 5.8.0 editor build.
- `AMSim.Phase4_5` and
  `AMSim.Phase1_5.Presentation.Paper2DWorld` automation.
- Aircraft and facility overview captures plus equivalent VA-01, VA-03, and
  VA-06 boards.
- Full rendered S06/S10 journey/save/state/capture regression.

