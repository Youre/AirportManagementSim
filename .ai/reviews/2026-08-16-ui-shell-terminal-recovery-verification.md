# UI shell and terminal recovery verification

Date: 2026-08-16
Result: Passed with continuing visual follow-up

## Verified outcome

- The new-airport onboarding dismissal and Create Airport action accept
  pointer input in the packaged application.
- Loading `VisualBaseline` returns to the persistent airport overview. It does
  not automatically enter Terminal, Regional, or another phase destination.
- Terminal and Regional are explicit destinations. Their visible Back actions
  and Escape both restore the overview and management camera.
- Restoring a save increments a non-serialized presentation epoch, so cached
  root, phase, terminal-layout, and Paper2D snapshots refresh even when the
  restored domain revision matches the prior in-memory revision.
- The terminal cutaway shows the schema-10 spatial layout at the same footprint
  scale as its generated roof. Legacy regional-terminal substitutes, mature
  site overlays, and screen-fixed room labels do not compete with it.
- Mixed-perspective terminal and hut artwork is no longer rotated. Those
  sources remain north-up pending strict-nadir or reviewed directional
  replacements.

## Automated and release evidence

- Full automation report:
  `AMSim/Saved/Automation/UIRecoveryFinal3/index.json`
  - 82 total
  - 81 succeeded
  - 1 succeeded with the retained Phase 0 warning
  - 0 failed, not run, or in process
- Phase 7 source audit:
  `AMSim/Saved/Phase7/release-audit.json`
  - 0 runtime string asset loads
  - 0 forbidden runtime dependencies
  - 0 required 3D asset candidates
  - 0 source line-limit violations
- Clean Development and Shipping packaging passed. The final exact-source
  package passed launch smoke and was copied to the canonical owner location.
- Canonical launcher: `D:/AMSim-Current/Windows/AMSim.exe`
- Canonical inner binary:
  `D:/AMSim-Current/Windows/AMSim/Binaries/Win64/AMSim.exe`
  - size: 228,076,032 bytes
  - SHA-256:
    `C1A1886CC92567DF7052228621D1C6BC54BFDD2B58F1183A88A5033AD9294AD7`

## Packaged interaction replay

The actual Windows application was exercised with pointer and keyboard input:

1. Dismiss new-airport onboarding.
2. Create the named airport and reach the starter-plan state.
3. Open Load and restore `VisualBaseline` to the root airport overview.
4. Open Terminal and inspect the spatial top-down terminal.
5. Press Escape and return to the overview.
6. Open Regional explicitly, inspect its schedule surface, and return to the
   overview.

The full interaction replay ran on the immediately preceding clean package.
The final package differs only by relocating two input-configuration methods
between source files to satisfy the 2,000-line standard; it then passed a clean
build, package, and launch smoke.

Representative captures:

- `AMSim/Saved/UsabilityRecoveryAudit/05-fixed-new-airport.jpg`
- `AMSim/Saved/UsabilityRecoveryAudit/06-fixed-created-airport.jpg`
- `AMSim/Saved/UsabilityRecoveryAudit/09-fixed-mature-terminal.jpg`
- `AMSim/Saved/UsabilityRecoveryAudit/VA03-terminal-recovery-comparison.png`

## VA-03 comparison assessment

The comparison confirms that the recovered view is now coherent, strictly
top-down, and functionally navigable. It does not yet match the concept's
content quality.

Highest-impact remaining differences:

1. The concept fills the terminal and surrounding airport with passengers,
   staff, vehicles, aircraft, curbside activity, and operational props; the
   implementation remains sparse.
2. The concept communicates passenger flow with strong continuous routes,
   room boundaries, gates, and security transitions; current overlays and
   room hierarchy are comparatively faint.
3. The concept integrates apron, aircraft, road, parking, and landscape into
   one composed scene; the current cutaway sits in a largely empty grass field
   with simpler side panels.

These are recorded as continuing VA-03 visual debt. The recovery deliberately
did not replace simulation, terminal topology, or construction mechanics while
repairing the unusable input and navigation shell.

## Storage and cleanup

- C: 126.84 GiB free
- D: 689.36 GiB free
- No shared Unreal or project cache was cleared.

