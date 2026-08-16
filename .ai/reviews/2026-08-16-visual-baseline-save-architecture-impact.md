# Visual-baseline save and HUD remediation architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

- The owner can load a named `VisualBaseline` airport through the ordinary
  Load menu and begin visual review from a populated, operational regional
  airport instead of rebuilding the starter airfield.
- The baseline is deterministic and can be regenerated from the existing Phase
  4 fixture after save-schema or presentation changes.
- The first remediation pass targets the shared VA-01 HUD shell and its visual
  hierarchy while leaving simulation behavior unchanged.

## Baseline-save contract

- The deterministic Phase 4 packaged smoke remains the authority for baseline
  simulation state; no parallel hand-edited simulation fixture is introduced.
- A project script generates the ordinary `Phase4Smoke` slot, validates the
  smoke result, then installs a copy as `VisualBaseline` under the actual
  portable package save root beside the canonical executable.
- The installed slot uses ordinary metadata and snapshot files, loads through
  `UAMSimGameInstanceSubsystem`, and migrates through the normal schema path.
- Existing save slots are never deleted or overwritten implicitly. Refreshing
  an existing `VisualBaseline` requires an explicit switch and retains a
  recoverable backup.
- Generated verification output stays under ignored `AMSim/Saved` evidence;
  only the script and written baseline contract enter source control.
- Save-slot discovery accepts metadata for every supported historical schema,
  allowing the ordinary snapshot restore path to perform its documented
  migration. Future-schema metadata remains hidden/rejected.

## Presentation boundary

- `AAMSimWorldPresenter` remains the sole airport-world renderer.
- The first visual pass is presentation-only: root CommonUI/UMG composition,
  shared theme tokens, concise copy, and contextual visibility.
- No facility, route, aircraft, passenger, economy, timetable, or progression
  state is inferred from widget layout.
- The same loaded snapshot, camera, viewport, UI scale, and screen remain fixed
  for every before/after comparison.

## Initial bounded remediation

- Use VA-01 as the primary target for the persistent gameplay shell.
- Reduce the footer from a destination/settings strip into contextual controls
  and feedback; keep save/load, time, accessibility/help, and destination
  access operable through grouped controls rather than permanent equal-weight
  buttons.
- Strengthen the concept hierarchy: airport identity, economy/rating, time,
  weather, and play state in the top bar; icon navigation at left; alerts,
  flights, and projects at right; selected-object detail at bottom only when
  context exists.
- Reuse the Riverbend palette, rounded surfaces, cooker-visible icon kit, and
  existing query-backed presenters. No placeholder symbol, 3D asset, runtime
  string load, or generated art is introduced in this pass.

## Compatibility and rollback

- Save schema 10 and schema 1-9 migrations remain unchanged.
- The baseline installer is development tooling and does not seed a save into a
  new player's profile or Shipping package.
- UI changes can be reverted independently of the baseline slot; the snapshot
  remains a valid ordinary save.

## Required proof

- Script-level generation, metadata, checksum, no-overwrite, backup, migration,
  and load checks.
- A real packaged Load-menu-to-overview capture using `VisualBaseline`.
- VA-01 before/after comparison at the same state, viewport, and camera.
- Cursor, click, focus, load, save, time controls, navigation, and compact-mode
  checks after the HUD mutation.
- Focused presentation/save tests, full automation, Development build/package,
  cooked dependency scan, and packaged smoke before handoff.
