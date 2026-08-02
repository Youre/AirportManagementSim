# Owner-smoke correction verification

Date: 2026-07-29 UTC

Status: implementation verified; exact packaged owner-flow retest pending

## Reported blockers and disposition

| Report | Correction | Evidence |
|---|---|---|
| Windowed mode was unavailable | Added a persistent `WINDOWED`/`BORDERLESS` control backed by `UGameUserSettings`; the corrected Development package was captured in a bordered 1600 x 900 window | `AMSim/Saved/Phase7/OwnerAudit-20260729/06-corrected-start-windowed-print.png` |
| Only the default runway could be accepted | Added connected whole-plan north/south/east/west/reset controls with authoritative validation; the road endpoint remains attached to the parcel boundary | `AMSim.Phase4_5.Presentation.ConstructionValidationContent` passed |
| Zoom worked but pan did not | Added middle-mouse screen-delta pan to the existing orthographic pawn while retaining WASD pan and parcel clamps | `AMSim.Phase0.Presentation.OrthographicCamera` passed |
| Accepted placement differed from the world and then stalled | World proxies now consume the committed proposal, including aircraft/support offsets; successful confirmation starts 1x time | `AMSim.Phase1_5.Presentation.Paper2DWorld` passed |
| Back actions and later destinations were not discoverable | Airport, Terminal, Regional, Advanced, and Major destinations remain visible; unavailable destinations are disabled and explicitly labeled `LOCKED`; nested views retain their return actions | Corrected packaged capture and compiled navigation path |
| Save/load had no visible selection | Added a persistent selected-slot control, existing-slot discovery, named empty slots, exact-slot save/load messages, and creation-time slot selection | Corrected packaged capture and compiled save-store path |

The Release Guide and its tabs required no correction.

## Verification performed

- `AMSimEditor Win64 Development` compiled successfully with Unreal Engine
  5.8.0.
- Focused tests passed:
  - `AMSim.Phase4_5.Presentation.ConstructionValidationContent`;
  - `AMSim.Phase1_5.Presentation.Paper2DWorld`;
  - `AMSim.Phase0.Presentation.OrthographicCamera`.
- The complete `AMSim` automation suite reported 67 succeeded, one retained
  warning-only legacy backup probe, zero failed, zero not-run, and zero
  in-process tests.
- A clean Development `BuildCookRun` succeeded and archived to
  `AMSim/Saved/OwnerAuditPackages/Development`.
- `git diff --check` passed for the correction set.

## Remaining gate

This does not revive the RC1 identity. The owner should repeat the exact short
new-airport path in the corrected package. After that passes, produce and audit
fresh Development and Shipping packages, record their hashes, and assign the
replacement release-candidate identity. The longer Phase 5-7 acceptance,
four-hour soak, and formal unassisted test remain pending as already planned.
