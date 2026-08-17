# Continuous terminal cutaway architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

The terminal is part of the airport map rather than a destination screen. The
normal map shows its generated roof. Focusing the terminal reveals the interior
at the same world anchor while the airport header, footer, nearby runway,
taxiway, apron, roads, facilities, aircraft, vehicles, and landscaping remain
present. The player can pan and zoom across the same continuous world.

## Affected boundaries

- `AAMSimWorldPresenter` remains the only airport-world renderer. Terminal
  cutaway changes only the terminal roof/interior visibility state; it no
  longer suppresses the rest of the mature airport.
- `AAMSimCameraPawn` retains one management camera and one map coordinate
  system. Terminal focus stores the previous framing and provides a closer
  starting frame, but preserves the full management pan and zoom range.
- `UAMSimRootScreen` retains the airport shell while the terminal overlay is
  active. Regional, Advanced, and Major operations may continue to use their
  existing destination surfaces; this correction is scoped to the physical
  terminal.
- `UAMSimTerminalView` becomes transparent contextual chrome over the live
  airport world. It owns terminal tools and inspectors but no replacement
  background, header, footer, or screen-fixed world labels.
- Phase 3 simulation, schema 10, terminal topology, construction, routing,
  economy, save/load, and migration contracts do not change.

## Interaction contract

- Terminal focus may center the camera, but cannot prevent the player from
  zooming back to the whole airport or panning through the normal map bounds.
- A bounded transparent map-interaction surface receives terminal placement
  gestures and right-drag panning without covering the persistent airport
  controls.
- Close Cutaway and Escape restore the generated roof and the exact prior
  management-camera frame.
- Build and Operations remain modes of the terminal overlay, not separate
  scenes.

## Compatibility and rollback

No save migration is required. Rollback is limited to root-shell visibility,
terminal overlay composition, camera limits, and world-proxy visibility.

## Required proof

- Focused automation proves that mature airport facilities remain visible
  while terminal floors and furnishings are visible.
- Camera automation proves terminal focus can use a contextual frame wider
  than the former 9,000-unit cap and restores the prior management frame.
- A packaged VisualBaseline journey captures overview, in-place cutaway, and
  restored overview at the same viewport.
- Full automation, clean Development and Shipping packages, launch smoke,
  release audit, and canonical package refresh pass.
