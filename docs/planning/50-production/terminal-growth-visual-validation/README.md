# Growable Terminal Visual Validation

**Status:** Implemented baseline accepted; continuing visual polish recorded
**Reference:** VA-03 Terminal and passenger flow
**Canonical state:** 1920 x 1080, 100 percent UI scale, ground-floor cutaway

## Approved targets and runtime evidence

| State | Approved target | Final runtime | Comparison board |
| --- | --- | --- | --- |
| Starter GA terminal editor | `target-01-starter-ga-terminal-editor.png` | `runtime-01-starter-ga-terminal-editor.png` | `comparison-01-starter-ga-terminal-editor.png` |
| Grown regional terminal operations | `target-02-grown-regional-terminal-operations.png` | `runtime-02-grown-regional-terminal-operations.png` | `comparison-02-grown-regional-terminal-operations.png` |

The owner approved both target frames on 2026-08-08. They remain design
evidence rather than simulation authority: runtime costs, counts, labels,
geometry, routes, and timings come from the revisioned simulation query.

## Implemented visual contract

- The terminal remains in the continuous orthographic airport world and uses a
  stable terminal anchor independent of runway-proposal or operations-hut
  movement.
- Selecting the terminal enters a ground-floor cutaway with generated roof
  removal, a one-meter grid, pooled floors, walls, doors, furniture,
  amenities, construction stages, workers, and visitors.
- Build and Operations are explicit modes. The Build rail uses compact icons,
  hover labels, a world-space placement ghost, price/status copy, rotate,
  demolish, and undo. Operations retains room, occupant, route, closure, and
  readiness context.
- The Riverbend rounded navy/cyan language, concise copy, visible cursor,
  right-drag pan, Escape/back behavior, and readable bottom controls are
  retained.
- At 175 and 200 percent the rail and contextual content use the compact drawer
  layout instead of reducing typography.

## Equivalent-state assessment

### Starter GA terminal editor

The bounded correction pass established a furnished, readable terminal instead
of the previous static roof sprite: public and service floor treatments,
perimeter/room edges, two gate doors, seating, information/restroom/service
objects, amenities, people, and world-scale editor controls all render from the
spatial query.

The three highest-impact remaining differences are:

1. The approved target includes richer road, apron, landscaping, and active
   construction-gesture detail; the deterministic runtime proof captures a
   clean baseline editor state without an in-progress pointer drag.
2. Runtime modular sprites are crisp and editable but less painterly and less
   densely dressed than the composition target.
3. Runtime preserves the project-standard icon rail with hover expansion,
   while the target holds category labels open for annotation clarity.

### Grown regional terminal operations

The Phase 3 fixture now uses the same spatial renderer with the entire grown
footprint in view, physical partitions, denser furniture, passengers, staff,
bags, vehicles, and the existing operational inspector. Passenger, baggage,
security, staffing, flight, and reconciliation behavior remain authoritative.

The three highest-impact remaining differences are:

1. The runtime terminal is materially more populated than the old fixture but
   still has less architectural and furnishing density than the painterly
   target.
2. Operational flow evidence uses current labels and entity traces rather than
   the target's thick colored route families and richer room-icon palette.
3. The target devotes more detail to apron aircraft and landside scenery;
   runtime prioritizes fitting the complete terminal and readable inspectors in
   the available world region.

These differences are continuing presentation debt, not missing vertical-slice
mechanics. The passenger construction catalog and upper floors remain explicitly
deferred.

## Responsive and packaged validation

- `AMSim/Saved/TerminalGrowth/ScaleMatrix/scale-matrix.json` passes the starter
  editor, grown passenger operations, and component gallery at 100, 125, 150,
  175, and 200 percent at 1920 x 1080. The 175-200 percent correction keeps
  the complete terminal visible behind small scrollable drawers.
- The final packaged Development proof exits cleanly and writes
  `AMSim/Saved/TerminalGrowth/starter-ga-terminal-build.png`; SHA-256
  `ff771f62737444f6c5055c1b77063783acdee878eaa99cafdcd7944552e4c697`.
- The grown Phase 4 rendered fixture passes journey, save/load, state,
  screenshot, reconciliation, and performance assertions.
- Cook inventory contains all 44 terminal textures and all 44 PaperSprite
  companions under `/Game/TerminalGrowth`.

## Intentional differences

- Game-state text and values replace incidental mockup values.
- The existing passenger simulation remains authoritative; the grid is its
  spatial/presentation bridge in this rollout.
- Passenger-processing objects are visible and operational but not yet a
  player-placeable catalog.
- No upper floors, 3D assets, real airline brands, or runtime string loads are
  introduced.
