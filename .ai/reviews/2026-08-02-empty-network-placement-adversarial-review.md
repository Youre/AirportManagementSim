# Empty network placement overhaul — adversarial review

Date: 2026-08-02

Status: controls required during implementation

| Failure case | Required control |
|---|---|
| Hidden seed geometry appears before placement | Gate every surface, handle, diagnostic, and inspector value by the presentation completion mask. |
| The first click creates a long rectangle | Show a fixed-size `START` circle only; a full surface is forbidden until the second endpoint is committed. |
| Click-drag and click-click fight each other | Treat first press as start, pointer movement as a non-authoritative preview, and either a meaningful drag release or second click as completion. |
| A zero-length accidental drag completes a path | Require a movement threshold and keep the tool at the end-placement step when the threshold is not met. |
| Taxi crossing is rejected on a rotated runway | Use orientation-independent segment intersection and point-to-segment distance in the authoritative validator. |
| A near miss is presented as a crossing | Derive crossing markers from the same authoritative geometry helper used by validation. |
| Moving a completed runway strands a taxiway silently | Revalidate immediately and attach a text-backed diagnostic to the disconnected taxi endpoint or nearest runway point. |
| Runway numbers disagree at opposite ends | Derive one `01`–`36` designator from start-to-end heading and the reciprocal by adding 18 modulo 36; test cardinal and diagonal cases. |
| Numbers appear during an unfinished gesture | Hide threshold markings and number labels until the runway completion bit is set. |
| Committed world geometry reverts to the authored layout | Map every stored point directly and test center, length, and rotation independently of the default fixture. |
| Undo restores geometry but not placed/empty state | Capture and restore proposal plus completion mask as one undo snapshot. |
| Confirm accepts hidden default fields | Require all five completion bits, no unfinished path, and authoritative validation. |
| Decorative handles are too small or color-only | Use large circular labeled targets, strong outline/focus state, and text instructions independent of color. |
| The overhaul regresses saves or later phases | Keep the schema unchanged and run the complete `AMSim` automation suite plus packaged replay. |

## Release posture

The prior build-mode candidate is superseded. A new candidate requires focused
geometry and interaction automation, complete regression automation, a clean
Shipping package, direct pointer replay from an empty parcel, and a VA-02
equivalent-state comparison.
