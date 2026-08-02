# Owner Smoke Correction Adversarial Review

**Decision:** proceed with focused controls

## Failure modes and controls

| Risk | Required control |
| --- | --- |
| Proposal controls create disconnected geometry | Translate the complete starter kit together and re-run the existing validator after every edit |
| Accepted and rendered placement diverge | Derive all Phase 1 facility transforms from the committed proposal and cover the mapping with automation |
| Funding appears stalled | On accepted commit, set 1x explicitly and show active construction feedback |
| UI consumes camera input | Support both Enhanced Input WASD and root-level middle-drag; ignore drag while a modal is open |
| Camera escapes useful world bounds | Clamp world offset and scale drag distance by orthographic width |
| Destination controls imply unavailable gameplay | Show locked buttons disabled with plain-language tooltips; never initialize a phase merely by navigating |
| Save selection overwrites an unexpected slot | Show the selected slot persistently, refresh known slots from metadata, and report the exact slot in save/load feedback |
| Window toggle produces an unusable size | Use 1600x900 windowed and desktop-sized borderless; persist through `UGameUserSettings` |
| Correction breaks Shipping boundary | Re-run cook inventory, forbidden dependency, runtime-load, 3D, and offline package audits |
| Automated fixture masks the manual path again | Add a packaged/manual-path smoke that uses the same proposal confirmation and visible controls as the player |

## Acceptance

The candidate cannot return to owner acceptance until an actual packaged run
proves custom placement, matching committed geometry, construction progress,
middle-drag pan, destination discoverability, window switching, and selected
slot save/load.
