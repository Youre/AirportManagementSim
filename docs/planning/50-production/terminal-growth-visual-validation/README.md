# Growable Terminal Visual Validation

**Status:** Implemented baseline accepted; continuing visual polish recorded
**Reference:** VA-03 Terminal and passenger flow
**Canonical state:** 1920 x 1080, 100 percent UI scale, roofless ground floor

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
  stable landside-campus anchor independent of runway-proposal or operations-
  hut movement.
- The ground-floor interior is always present with no generated roof: a
  one-meter logical grid, pooled floors, walls, doors, furniture,
  amenities, construction stages, workers, and visitors.
- Each logical grid cell maps to a sixteen-meter airport-world presentation
  module. The furnished starter footprint is approximately 288x192 meters and
  the grown regional footprint is approximately 480x288 meters. Static
  furniture uses an aspect-preserving 84-percent fill of its full authored
  footprint; people, bags, workers, and vehicles use a separate twelve-times
  readability scale.
- Build and Operations are explicit modes. The Build rail uses compact icons,
  hover labels, a world-space placement ghost, price/status copy, rotate,
  demolish, and undo. Operations retains room, occupant, route, closure, and
  readiness context.
- The Riverbend rounded navy/cyan language, concise copy, visible cursor,
  right-drag pan, Escape/back behavior, and readable bottom controls are
  retained.
- At 175 and 200 percent the rail and contextual content use the compact drawer
  layout instead of reducing typography.
- Seating uses one strict-nadir project-owned module composed into the exact
  two-, four-, and six-seat counts authored by the terminal layout. Opposing
  rows face opposite directions. Every other furnishing preserves its source
  aspect ratio while fitting its rotated simulation footprint.

### Single-canvas boundary

The root airport map remains the only world after loading a save and while
Terminal tools are open. The retired fixed-coordinate Phase 3 terminal proof
renderer is never a root-level view. The schema-10 interior remains roofless
and visible in the airport world at all times. Opening and closing Terminal
tools preserves the runway, apron, roads, aircraft, landscaping, terminal
geometry, root shell, and player-owned camera frame. Large mature facilities
use deterministic procedural base, surface, and detail geometry; small
aircraft, people, vehicles, fixtures, and landscaping retain reviewed sprites
where their silhouettes carry useful identity.

### Doubled terminal-campus correction

The first eight-meter correction still read too small in owner review and is
superseded. The presentation now uses the same sixteen-meter module for floor
proxies, walls, construction treatments, pointer deprojection, placement
previews, and physical-size feedback. A fixed presentation-only anchor places
the doubled footprint between the apron and access road. It does not change
simulation cells, costs, routes, save schema, camera state, or renderer
ownership.

The packaged equivalent-state record is
`double-scale-terminal-va01-comparison.png`; its implementation source capture
is `double-scale-terminal-packaged.png`. The earlier `normal-scale-*` artifacts
remain historical evidence of the rejected intermediate scale.

### Interior-content proportion correction

The first doubled-campus capture exposed a second proportion error: the
building was large enough, but furnishings and moving identities still read as
dots. Static objects and exact seat compositions now fit against the full
sixteen-meter presentation cell while retaining the existing 84-percent safe
inset and uniform aspect ratio. People, bags, workers, and service vehicles use
a smaller twelve-times identity token so they remain subordinate to furniture
and room boundaries.

The identical-state before/after record is
`scaled-content-terminal-before-after.png`; the updated VA-01 board is
`scaled-content-terminal-va01-comparison.png`; and the packaged source capture
is `scaled-content-terminal-packaged.png`. Object anchors, counts, rotations,
routes, occupancy, schema, simulation, camera, and renderer ownership do not
change.

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
  `8c2995dac8d39e6ea4270144af7c5203c72d9f8acae622499329c5ce4cd0969e`.
- The grown Phase 4 rendered fixture passes journey, save/load, state,
  screenshot, reconciliation, and performance assertions.
- Cook inventory contains all 45 terminal textures and all 45 PaperSprite
  companions under `/Game/TerminalGrowth`.

## Intentional differences

- Game-state text and values replace incidental mockup values.
- The existing passenger simulation remains authoritative; the grid is its
  spatial/presentation bridge in this rollout.
- Passenger-processing objects are visible and operational but not yet a
  player-placeable catalog.
- No upper floors, 3D assets, real airline brands, or runtime string loads are
  introduced.
- VA-01 still gives the terminal and landside campus substantially richer
  architectural, material, and activity density. The doubled runtime footprint
  now has comparable visual prominence; further composition and dressing remain
  a separate bounded visual pass.
