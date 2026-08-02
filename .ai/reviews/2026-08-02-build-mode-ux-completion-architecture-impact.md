# Build-mode UX completion — architecture impact

Date: 2026-08-02

Status: approved for implementation

## User-visible outcome

Complete the direct-placement correction so a first-time player can see the
owned parcel, place and repair each starter-airfield part with the mouse,
understand the exact invalid location, undo an uncommitted edit, and commit the
same proposal they inspected. Normal gameplay rails and the global footer are
temporarily removed while build mode owns the decision.

## Non-goals

- No new facility definitions, construction stages, prices, save fields, or
  Phase 2–7 gameplay.
- No change to `ValidateStarterPlan` or the `CommitStarterPlan` authority.
- No arbitrary curved networks, multi-proposal persistence, demolition, or
  complete mature-airport catalog in this starter-flow correction.

## Decisions

1. `UAMSimConstructionProposalView` remains the sole owner of temporary
   proposal geometry, tool/path state, hover preview, undo snapshot, pointer
   capture, endpoint editing, and validation presentation.
2. `AMSim::ValidateStarterPlan` remains authoritative. Presentation derives
   localized diagnostics from its stable reason code and the proposal geometry;
   those diagnostics cannot override the command result.
3. Runway and taxiway support two-click endpoint placement and direct endpoint
   dragging. Stand/work zone, operations hut, and road access are distinct
   visible tools and mutate only their explicit `FStarterPlanProposal` fields.
4. The proposal widget publishes only a build-mode visibility delegate.
   `UAMSimRootScreen` responds by collapsing its normal left/right rails and
   footer, preserving the persistent header and expanding the map surface.
5. All proposal overlays are deterministic UMG presentation. No new runtime
   asset path, Blueprint dependency, save schema, or simulation feedback path is
   introduced.
6. Confirmation submits the current proposal once through the existing command;
   cancel, reset, undo, hover, and invalid edits remain presentation-only.

## Compatibility and rollback

- Save schema 8 and existing saves are unchanged.
- The default valid proposal remains available as a safe recovery path.
- Rollback is limited to the proposal widget, its focused tests, and the root
  chrome-visibility binding; no authoritative data migration is required.
- The existing packaged first-run window preference and camera behavior remain
  untouched.
