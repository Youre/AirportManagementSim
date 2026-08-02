# Owner interaction redesign — adversarial review

Date: 2026-07-29 UTC

Status: controls required during implementation

## Failure cases and controls

| Failure case | Required control |
|---|---|
| Right-click opens or activates UI while attempting to pan | Start right-drag only on the world surface; suppress pan while modal, guide, construction, or close-operations views own input |
| Keyboard pan still jumps past the site | Reduce each action from 5,000 to 1,250 world units and retain parcel clamps |
| A facility tool silently moves dependent geometry | Tool selection must identify exactly what will move; validation must update after every edit |
| Manual placement creates a disconnected plan with no recovery | Show cause/remedy continuously, disable funding while invalid, and retain reset-to-starter-layout |
| Schedule picker offers a slot that expires before submission | Generate choices at least two timetable increments ahead and revalidate on selection |
| Rejected scheduling remains unexplained | Map command result to actionable player copy and keep the picker open on rejection |
| Load menu lists nonexistent or corrupt slots as usable | Populate from valid metadata only; disable empty rows and state that no save exists |
| Quick save overwrites an unexpected slot | Creation and successful load set the active slot; save confirmation names it |
| New modal traps focus or obscures dismissal | Provide a visible cancel/close action and restore gameplay input/cursor state |
| Footer becomes more crowded | Remove the persistent slot control rather than adding another menu button |

## Release posture

RC1 remains superseded. This correction requires focused automation, complete
automation, a clean Development package, and an exact owner-flow retest before
replacement Shipping packaging.
