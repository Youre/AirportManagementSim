# First-visit control loop — verification

Date: 2026-08-03

## Result

Passed technical verification. Owner replay remains the subjective interaction
gate.

## Evidence

- Unreal Engine 5.8 Editor target compiled successfully.
- `AMSim.Phase1.Presentation.FirstVisitOperationsHub` passed 1/1 with no
  warnings. It covers the real-time inbound countdown, automatic-operation
  copy, committed-build status, overlay text identity, and Living Airport
  continuation.
- `AMSim.Phase1_5.Presentation.Paper2DWorld` passed 1/1 with no warnings. It
  proves Connections and Activity become distinct presenter states, change the
  world treatment, and restore the standard treatment.
- Complete `AMSim` automation passed 75 tests plus the retained one
  succeeded-with-warning missing-save recovery probe; zero failed, not-run, or
  in-process tests.
- Development BuildCookRun succeeded with 658 cooked packages and produced the
  corrected package at `D:\AMSimPlayable-20260803-FirstVisit\Windows\AMSim.exe`.
- The packaged build launched windowed and displayed the expected four-action
  icon rail. A new-path Windows Firewall permission prompt blocked further
  computer-use interaction; it was neither accepted nor dismissed by the
  agent. Two hidden packaged-smoke attempts also remained held behind that
  prompt and were terminated at their 90-second bound; they are not counted as
  passing evidence. The post-construction owner replay therefore remains
  required.
- Root presenter remains at 1,999 lines. No source/header file exceeds the
  project 2,000-line limit.

## Required owner replay

Dismiss the Windows Firewall prompt according to the owner's preference, load
or complete the starter build, then confirm:

1. Build opens committed-airfield status rather than becoming dead.
2. Schedule opens the first-visit card and scheduling pauses time.
3. Watch at 1x and Advance to arrival both work; advance returns to 1x inbound.
4. Continue at 4x accelerates the visible operation without hiding its state.
5. All three overlay modes visibly switch and restore.
6. Completion exposes `START LIVING AIRPORT` as the next action.
