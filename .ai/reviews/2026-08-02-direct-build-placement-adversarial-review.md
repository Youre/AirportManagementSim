# Direct build placement — adversarial review

Date: 2026-08-02

Status: controls required during implementation

## Failure cases and controls

| Failure case | Required control |
|---|---|
| Clicking a tool also places geometry beneath the button | Exclude compact chrome rectangles from world-placement capture and let child buttons consume their click. |
| A drag jumps the selected item to an unrelated location | Drag applies the snapped pointer delta to a captured proposal snapshot; a click without movement intentionally centers the selected item at the cursor. |
| Placement is still constrained to the old center preview | Map the owned parcel across nearly the full construction surface and accept cursor positions beyond it so invalid out-of-parcel previews remain diagnosable. |
| Runway, taxiway, or stand silently moves another tool | Each tool translates only its owned proposal fields; the fixed public-road endpoint remains attached to the parcel boundary. |
| Tiny pointer motion causes noisy proposal churn | Snap proposal points to the existing 10 m planning grid and refresh only when the snapped geometry changes. |
| Invalid placement can be funded | Re-run `ValidateStarterPlan` after every change and disable the build action while invalid. |
| The reason for a disabled build action is hidden | Show one live, color-independent issue card with the validator cause and remedy, and label the disabled action `FIX PLACEMENT TO BUILD`. |
| The player cannot leave build mode | Keep `CANCEL` visible at all times and bind Escape to the same non-mutating exit. |
| Confirmation is ambiguous | Use a persistent positive action labeled `BUILD AIRFIELD · 3,400 CR`; on acceptance, start 1× time and close build mode. |
| Default window mode overrides a later preference | Set only the packaged first-run defaults; do not force window mode every launch. |
| Direct manipulation changes authoritative state before commitment | Keep the proposal local to the widget and submit exactly once through the existing Phase 1 command. |

## Release posture

RC1 remains superseded. This interaction correction requires a clean editor
compile, focused/full automation, a new Development package, and a Computer Use
replay of open build mode → place three tools → resolve validation → confirm.
