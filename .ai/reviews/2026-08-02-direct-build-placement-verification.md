# Direct Build Placement Verification

**Date:** 2026-08-02 EDT
**Source identity:** `d69c29d5c9f906c150a78e77d7b6d99554345926` plus the uncommitted direct-placement correction
**Outcome:** Passed for a Development candidate; owner replay and replacement Shipping verification remain open.

## Verified behavior

- A fresh packaged launch opens as a 1600x900 window with normal Windows chrome.
- Build mode uses a compact runway, taxiway, and stand/service tool palette.
- The selected runway can be dragged across the world and the taxiway can be placed by clicking the world.
- Placement remains grid-snapped and the Phase 1 simulation validator remains authoritative.
- Invalid placement changes the plan card to a text-backed `PLACEMENT ISSUE`, supplies a cause/remedy, and disables the confirmation action as `FIX PLACEMENT TO BUILD`.
- `RESET LAYOUT` restores the valid connected starter arrangement.
- `CANCEL` and the persistent `BUILD AIRFIELD - 3,400 CR` action are visible without opening another panel.
- Confirmation commits the proposal, deducts the expected cost, advances at 1x, and produces the constructed airfield.
- The former nudge controls are absent from source and packaged presentation.

## Automated and build evidence

- Clean `AMSimEditor Win64 Development` compile: passed.
- Focused construction-presentation automation: 1 succeeded, 0 warnings, 0 failures.
- Complete AMSim automation: 67 succeeded, 1 retained warning-only backup probe, 0 failures, 0 not-run.
- `BuildCookRun` Development package: passed.
- Package size: 594,286,902 bytes (0.553 GiB).
- Launcher SHA-256: `8E61AE1487664610F75BCC9A2439DCBD48541F8CEEAEF88A913BF9EE5A2F0CD5`.
- Runtime SHA-256: `5A6DBDCFAF2C8AE4CDFEFA27D86A42057F8D967A30FFC1AADD41AF2FD99FC6F1`.
- Construction presenter: 897 lines; nudge matches: 0; `git diff --check`: passed.

## Visual and interaction evidence

Evidence is retained under `AMSim/Saved/Phase7/BuildModeAudit-20260802/`:

- `01-current-build-mode.png` - pre-correction clutter and nudge-control baseline;
- `02-direct-build-mode.png` - compact full-map placement state;
- `03-live-placement-validation.png` - actionable invalid-placement state;
- `04-confirmed-construction.png` - successful committed result.

The packaged replay used the real Windows application and direct pointer input. It verifies visible hierarchy, cursor presence, button activation, world click/drag behavior, and the successful command path. Screenshot inspection does not replace owner judgment of placement feel, complete keyboard/focus accessibility, or the remaining Phase 7 consolidated acceptance protocol.

## Cache evidence

The read-only post-package tracker reports `Healthy`, 237.57 GiB free, 6.40 GiB of tracked project caches, and 0.51 GiB of tracked shared Unreal caches. No cache was deleted automatically.
