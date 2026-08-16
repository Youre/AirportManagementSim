# UI shell and terminal recovery preliminary adversarial review

Date: 2026-08-16
Status: Approved with controls below

## Failure cases and controls

1. **Transparent canvas also disables its buttons.** Use
   `SelfHitTestInvisible`, not `HitTestInvisible`, so rail children remain
   interactive.
2. **Loading retains an old destination.** Close terminal and phase
   presentations only after a successful restore, then refresh the root.
3. **Terminal button still opens Regional.** Destination is explicit UI state;
   availability checks may enable buttons but cannot rewrite that state.
4. **Regional Back leaves the camera in cutaway.** Non-terminal destinations
   disable cutaway before showing their full-screen view; closing any
   destination restores the saved management camera.
5. **Terminal shell stays collapsed after visiting Regional.** Restore all
   terminal-shell top-level children before applying conditional planning and
   label visibility.
6. **Escape is only decorative copy.** Handle Escape in both the root and the
   terminal destination, while retaining the visible Back control.
7. **Mature infrastructure reappears during a terminal revision.** Every mature
   visibility path includes the terminal-cutaway exclusion, and cutaway
   refresh reapplies it.
8. **Closing cutaway permanently hides the mature airport.** Re-run the
   authoritative mature/Phase 3 visibility policy when cutaway closes.
9. **Roof and interior drift.** Use the same cell-to-world transform and
   presentation scale for both; only height and visibility differ.
10. **Room labels float while panning or zooming.** Screen-fixed Phase 3 labels
    are hidden for the schema-10 cutaway until query-backed world labels exist.
11. **Furniture rotation makes perspective art look worse.** Do not newly
    rotate mixed-perspective facility art. Retain deterministic rotatable
    structure pieces and record directional furniture variants as visual debt.
12. **A broad visual rewrite regresses gameplay.** Do not change terminal
    topology, construction commands, costs, passengers, baggage, phase
    progression, or schema records in this recovery.
13. **Automation passes while packaged input remains blocked.** Complete the
    actual Windows pointer journey in the canonical package after packaging.
14. **Unrelated workspace files are staged.** Preserve the existing untracked
    `docs/3d_workflows/` and prior audit captures.

## Required proof

Focused presentation automation, the complete automation suite, Development
and Shipping packaging, release scans, and a computer-use replay of the exact
reported journeys are required before closeout.
