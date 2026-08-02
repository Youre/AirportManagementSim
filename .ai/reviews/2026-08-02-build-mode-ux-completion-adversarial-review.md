# Build-mode UX completion — adversarial review

Date: 2026-08-02

Status: controls required during implementation

| Failure case | Required control |
|---|---|
| Parcel copy and visible boundary disagree | Derive parcel mapping, border, grid, and point placement from the same normalized constants. |
| A hidden facility moves with another tool | Give stand, operations hut, and road access separate tool identities and field-specific translation tests. |
| A two-click path accidentally commits after one click | Keep the intermediate endpoint only in the uncommitted proposal, show the pending step, and leave confirmation governed by full validation. |
| Endpoint handles are decorative | Hit-test selected runway/taxi/road handles before whole-object translation and exercise handle edits in automation and packaged replay. |
| Snap targets imply a connection that validation rejects | Snap only to the authoritative runway centerline, stand center, and external road boundary rules used by `ValidateStarterPlan`. |
| Hover changes simulation or proposal state | Render the ghost from a derived candidate copy; mutate `CurrentProposal` only on an actual click or drag edit. |
| Dragging overwrites undo repeatedly | Capture one pre-edit proposal on mouse down and retain it for the entire drag gesture. |
| A generic error still forces panel reading | Place coral, text-backed markers on the exact out-of-bounds or disconnected point and keep the rail remedy concise. |
| Build mode leaves duplicate HUD controls interactive | Collapse the normal gameplay rails and footer for the proposal lifetime and restore their responsive visibility on every close path. |
| Closing after commit restores stale build chrome | Drive restoration from one visibility delegate called by confirm, cancel, Escape, and forced close. |
| Right-click pan is swallowed | Handle only left-button placement; continue bubbling other pointer buttons to the root/camera input path. |
| Tool content overflows at supported scales | Keep palette copy short, use bounded rails, and verify the supported scale matrix or record any remaining scale blocker. |
| Visual similarity hides a gameplay regression | Re-run authoritative Phase 1 construction tests, the complete automation suite, and a packaged create/place/repair/commit journey. |

## Release posture

The current Development candidate is superseded by this UX completion pass.
A new candidate requires a clean compile, focused and complete automation, a
clean package, real pointer/keyboard replay, and an equivalent-state VA-02
comparison with no actionable P0–P2 finding.
