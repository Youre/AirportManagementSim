# Owner interaction redesign — architecture impact

Date: 2026-07-29 UTC

Status: approved for implementation

## Scope

Correct four owner-observed interaction failures without changing Phase 1
simulation rules:

- map navigation;
- starter-airfield placement;
- first-flight timetable selection;
- save/load navigation.

## Decisions

1. Camera input remains presentation-only. Keyboard pan uses a smaller bounded
   step; right-mouse drag and middle-mouse drag both call the same clamped
   orthographic pan operation.
2. The construction presenter owns tool selection and editable proposal
   geometry. It submits only the completed `FStarterPlanProposal`; the
   simulation validator remains the sole authority for parcel, geometry,
   connectivity, and cost.
3. Scheduling gains a modal timetable picker derived from the simulation clock
   and Phase 1 timetable increment. The existing schedule command and validator
   remain authoritative.
4. Save-slot discovery remains in `UAMSimGameInstanceSubsystem`. The persistent
   footer slot selector is removed. `SAVE` quick-saves the active airport slot;
   `LOAD` opens a modal showing existing saves and loads only the selected row.
5. The new timetable and load surfaces are separate bounded overlays so the
   near-limit root presenter does not absorb another large layout.

## Non-goals

- no new construction types;
- no freeform runway length or heading editor in this pass;
- no new timetable or stand-capacity simulation;
- no deletion, renaming, cloud sync, or thumbnail saves;
- no changes to schema 8.

## Compatibility

Existing saves, deterministic replay, economy authority, asset references, and
Phase 1-7 simulation behavior remain unchanged. Presentation state is not
serialized except for the already-selected active slot.
