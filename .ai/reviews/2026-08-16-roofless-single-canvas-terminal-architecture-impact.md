# Roofless single-canvas terminal architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

The airport has one persistent world, one persistent camera, and one
authoritative renderer. The schema-10 terminal is always roofless, so its
interior remains visible at its real airport-world anchor in every gameplay
mode. Opening Terminal adds contextual tools and information only. It does not
move, zoom, replace, restore, or otherwise take ownership of the camera.

## Affected boundaries

- `AAMSimWorldPresenter` remains the only airport-world renderer. Its spatial
  terminal floor, edges, furniture, workers, visitors, and construction state
  remain active whenever a terminal snapshot exists. Generated roof proxies
  remain pooled but hidden.
- `AAMSimCameraPawn` has one player-owned pan/zoom contract. Facility modes do
  not retain alternate camera positions. Its global zoom range must support
  both airport overview and interior inspection without requiring a terminal
  mode.
- `UAMSimTerminalView` remains screen-space contextual chrome. Opening and
  closing it changes tool visibility and interaction state only.
- Phase 3 simulation, schema 10, terminal topology, construction, routing,
  economy, save/load, and migrations do not change.

## Non-goals

- Do not enlarge or relocate the terminal in simulation state.
- Do not add a terminal level, render target, minimap, second camera, or
  screen-space copy of the airport.
- Do not change Phase 1-7 gameplay or persistence contracts.

## Compatibility and rollback

No save migration is required. Rollback is limited to presentation visibility,
terminal tool state naming, and the camera zoom curve.

## Required proof

- Focused automation proves the spatial interior is visible before, during,
  and after terminal-tool use while every roof proxy remains hidden.
- Camera automation proves terminal-tool toggles never move or zoom the camera.
- Player-controlled zoom retains the full overview maximum and reaches the
  close inspection minimum without a facility-specific mode.
- Full automation, Development and Shipping packaging, release audit, and a
  packaged same-frame visual replay pass before canonical refresh.
