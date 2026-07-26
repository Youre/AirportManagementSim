# Camera, Controls, and Time

**ID:** PX-02  
**Status:** Approved  
**Depends on:** TS-02, TS-03, TS-06  
**Integration phase:** Phase 0 and Phase 1

## Outcome

Mouse and keyboard users can move rapidly between airport-wide planning and close observation, select any inspectable entity, build accurately, and control simulation speed without losing input context.

## Platform and input baseline

The approved first platform is Windows desktop with mouse and keyboard. Enhanced Input owns action mapping. Actions are semantic and remappable; gameplay code cannot read raw keys directly.

Required action groups:

- camera pan, edge pan, zoom, rotate-to-north, focus selection;
- primary select, additive select, cancel, context action;
- build confirm, rotate, mirror, delete proposal, eyedropper;
- pause, 1×, 2×, 4×, 8×;
- open build catalog, schedule, finances, alerts, overlays, help;
- cycle aircraft, flights, incidents, and construction projects;
- screenshot mode and UI toggle.

Controller support is not an acceptance requirement, but action definitions cannot prevent a later controller mapping.

## Camera model

The gameplay camera is orthographic and north-up by default.

- Zoom is continuous within configured limits.
- Camera pitch is fixed.
- Optional 90-degree world rotation is not supported in the first release; runway headings and screen-space labels remain unambiguous.
- Pan uses middle-mouse drag, WASD, and edge pan.
- Zoom anchors toward the mouse position when possible.
- Double-clicking or pressing focus centers the selected entity without changing time speed.
- Following a moving entity is explicit and ends on manual pan, Escape, entity despawn, or selecting another entity.
- Map bounds include a configurable margin and never expose unrendered space.

Zoom bands:

- region: parcels, runways, traffic, major alerts;
- airport: facilities, stands, roads, projects, service coverage;
- local: agents, room details, paths, service tasks;
- inspect: individual sprite, labels, movement, current task.

Each presentation system declares minimum and maximum visible zoom bands. Logical simulation is unaffected by zoom.

## Selection

Selection resolves by priority:

1. visible alert or interaction handle;
2. construction preview;
3. inspectable moving entity;
4. facility or room;
5. network segment;
6. ground cell.

When entities overlap, repeated clicks cycle candidates and a small chooser appears after the second ambiguous click. Selection uses stable simulation IDs, not Actor references.

## Time controls

The simulation supports pause, 1×, 2×, 4×, and 8×.

Initial time mapping:

- 1×: one real second advances one game minute;
- 2×, 4×, and 8× multiply game-time advancement;
- authoritative simulation uses fixed logical steps;
- presentation interpolates between logical states;
- opening menus does not automatically pause, except save/load, region selection, and blocking confirmation dialogs;
- the player can choose “pause on critical alert,” enabled by default.

Construction planning, inspection, scheduling, policy changes, and queue editing are available while paused. Accepted commands apply at the next simulation boundary; commands submitted while paused apply in deterministic order before time resumes.

The game performs no offline progression while closed.

## Feedback

- Current speed is always visible.
- The Windows mouse cursor remains visible while interacting with the world, HUD, and management panels, including during click capture.
- Keyboard shortcuts produce a brief non-modal label.
- A blocked speed change explains the reason, such as save migration or active blocking dialog.
- Critical incidents automatically reduce 8× or 4× to 1× when pause-on-critical-alert is disabled.
- Following an entity at high speed retains smooth interpolation but does not alter simulation.

## Acceptance criteria

- All required actions can be remapped without code changes.
- Clicking into the gameplay viewport does not hide or lock the mouse cursor.
- The player can reach any owned parcel, select an entity, and return north-up using only documented actions.
- Zooming from full airport to an individual aircraft preserves the mouse target within a small screen-space tolerance.
- Commands issued while paused apply in submission order when resumed.
- At each speed, a seeded simulation reaches identical authoritative state at the same game timestamp.
- Closing and reopening the game does not advance the calendar.
