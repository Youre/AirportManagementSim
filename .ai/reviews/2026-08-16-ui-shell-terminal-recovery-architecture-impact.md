# UI shell and terminal recovery architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

- The airport-name field and Create Airport action accept pointer input.
- Loading a save returns to the normal airport overview instead of opening a
  phase destination automatically.
- Terminal, Regional, Advanced, and Major are explicit destinations. Opening
  one cannot silently substitute another.
- Every full-screen destination retains a visible Back action, and Escape
  returns to the airport overview.
- Terminal cutaway shows one spatial terminal at one scale. It does not combine
  the schema-10 layout with mature-airport sprites, legacy terminal geometry,
  or screen-fixed room labels.

## Root causes being retired

- The full-size right activity canvas is hit-testable even outside its rail,
  so it intercepts the map and the airport-creation card underneath it.
- `RefreshFromSimulation` automatically opens the terminal for any initialized
  Phase 3 save, which makes loading change destination without player intent.
- `UAMSimTerminalView` selects Regional whenever Phase 4 is available, even
  when the player explicitly selected Terminal.
- Regional presentation collapses every terminal-shell child and does not
  reliably restore them, which can leave a world-only screen with no Back UI.
- Terminal cutaway refreshes the spatial layout without suppressing mature
  infrastructure and the legacy regional-terminal sprite.
- The generated roof uses a three-times larger footprint than the interior,
  so entering cutaway changes the apparent terminal scale.

## Target boundary

- `UAMSimRootScreen` remains the persistent application shell and owns
  explicit destination selection. Background rail canvases are
  self-hit-test-invisible; only their actual controls receive input.
- Loading restores simulation state, closes transient destinations, and leaves
  the root overview visible.
- `UAMSimTerminalView` keeps presentation destination as UI-only state:
  Terminal, Regional, Advanced, or Major. Simulation unlocks enable a
  destination but never choose it.
- Terminal cutaway continues to consume the immutable schema-10 snapshot.
  The world presenter suppresses mature/legacy proxies while cutaway is active
  and restores their authoritative visibility when it closes.
- The schema, economy, simulation, and save contents do not change.

## Asset projection policy

- `T_RegionalTerminal` is mixed-perspective artwork and is not a rotatable
  world primitive. The schema-10 generated roof is authoritative wherever a
  spatial terminal exists.
- Mixed-perspective building sprites remain fixed north-up until they are
  replaced by true nadir or authored directional variants.
- Floors, walls, doors, overlays, and other deterministic terminal pieces
  remain the rotatable construction vocabulary.

## Compatibility and rollback

- Schemas 1-10 and all phase command contracts are unchanged.
- Existing saves load without migration and now land at the overview.
- Rollback is limited to UI destination state and presenter visibility; no save
  data or asset rewrite is involved.

## Required proof

- Focused tests cover hit-test-transparent rail canvases, explicit destination
  selection, roof/interior footprint parity, and mature-proxy suppression in
  cutaway.
- A packaged replay covers new-airport creation, VisualBaseline load, Terminal
  open/Back/Escape, Regional open/Back, and root navigation responsiveness.
- The terminal capture is compared with VA-03 at the same viewport and state.
