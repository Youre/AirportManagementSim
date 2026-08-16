# Terminal art orientation architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

Terminal furniture, amenities, and room fixtures render upright relative to
the terminal shell and VA-03. A player-authored Rotate action still advances
the selected object by one quarter turn.

## Root cause

The terminal layout maps logical grid axes into the airport world's X/Y axes.
The retained terminal object textures are authored upright in screen space,
but their presenter applied `QuarterTurns * 90` directly. Zero-turn objects
therefore appeared one quarter turn counter-clockwise in the top-down camera.

## Boundary

- Add one clockwise 90-degree presentation-basis correction to placed terminal
  object proxies.
- Do not rewrite source textures, imported sprites, saved `QuarterTurns`,
  footprints, anchors, interaction ports, routes, or simulation state.
- Do not apply the correction to grid-aligned floors, walls, or doors; their
  yaw is derived from world geometry and already aligns to their edges.
- Treat a non-empty schema-10 terminal layout as sufficient presentation
  authority. A direct save restore must not require a prior Phase 1 presenter
  refresh before its roof or cutaway can render.
- Do not claim that rotating the retained pseudo-perspective object artwork
  makes it strict nadir. Nadir replacement remains separate visual debt.

## Compatibility and rollback

Schemas 1-10 remain byte-compatible. The correction is presentation-only and
can be reverted by removing the shared yaw offset.
