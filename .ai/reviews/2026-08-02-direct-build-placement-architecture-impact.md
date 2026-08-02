# Direct build placement — architecture impact

Date: 2026-08-02

Status: approved for implementation

## Scope

Replace the starter-airfield nudge-panel interaction with direct mouse placement
over the map and make windowed mode the first-run default. The change is
presentation and input only; Phase 1 validation, cost, construction, persistence,
and deterministic simulation remain authoritative and unchanged.

## Decisions

1. `UAMSimConstructionProposalView` owns the temporary proposal, active tool,
   pointer capture, grid snapping, live validation copy, and commit/cancel UI.
2. A selected runway, taxiway, or stand/service group can be repositioned by
   clicking or dragging anywhere on the unobscured map. Proposal coordinates
   continue to use `FPhase1Point` and submit through the ordinary
   `CommitStarterPlan` command.
3. The simulation's `ValidateStarterPlan` remains the only authority for parcel
   bounds, minimum dimensions, connectivity, and the quoted cost. The presenter
   can disable commitment but cannot manufacture a valid result.
4. Build chrome is reduced to a compact tool palette, one live validation card,
   and a persistent cancel/build action bar. Informational cards that do not
   affect the starter decision are removed from this surface.
5. The existing proposal preview stays presentation-only and expands across the
   map surface. It never writes geometry into simulation before confirmation.
6. `DefaultGameUserSettings.ini` establishes a 1600×900 windowed first-run
   default. Later player-selected window modes remain ordinary saved settings.

## Compatibility and boundaries

- Save schema 8 is unchanged.
- Existing committed proposals and saves retain their geometry.
- No new facility, economy, scheduling, or construction-stage behavior is added.
- C++ owns the durable input and validation bridge; UMG remains the presentation
  surface, consistent with the project's Unreal workflow guidance.
- The implementation must remain usable with a visible cursor and must preserve
  normal right/middle map panning outside modal-owned placement input.
