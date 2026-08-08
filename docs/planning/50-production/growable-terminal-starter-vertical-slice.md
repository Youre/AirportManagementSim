# PI-11: Growable Terminal Starter Vertical Slice

**Status:** Implemented and technically verified
**Owner:** Terminal construction, presentation, and integration
**Opened:** 2026-08-08
**Roadmap placement:** Post-Phase-7 product remediation

## Outcome

Replace the static starter-terminal presentation with a ground-floor,
one-meter-grid terminal that remains part of the airport world. Selecting the
terminal and zooming through the cutaway threshold fades its generated roof and
reveals the editable interior without loading a separate management scene.

The first rollout includes a furnished GA terminal, immediate-spend editing,
local construction closures, active construction workers, autonomous GA visitor
movement, schema-10 persistence, and a spatial rendering bridge for the
existing Phase 3 passenger fixture.

## Locked behavior

- The editor is hybrid: players draw floors and walls, then place rotatable
  footprint-based doors, furniture, amenities, and operational objects.
- New airports begin with a completed editable GA terminal containing an
  entrance, lobby, seating, information desk, accessible restroom route,
  staff/service space, and two airside gate doors aligned to the starter gate
  anchors.
- A valid pointer gesture spends immediately from the Phase 1 economy ledger.
  Invalid and unaffordable gestures do not mutate state.
- Undo fully refunds queued or in-progress work. Completed work leaves the undo
  history and uses demolition with a balance-owned 25 percent salvage default.
- Construction workers travel to local jobs, remain visibly active, face their
  movement direction, and advance proposal, delivery, building, inspection,
  and completion presentation states.
- Remodeling closes only affected cells. An edit is rejected when it strands an
  occupant, removes required egress, breaks an active gate route, or invalidates
  a controlled transition.
- The first rollout is ground-floor-only. Upper floors, stairs, elevators, and
  vertical routing remain approved future scope under GS-08 but are not claimed
  by PI-11.
- Existing passenger-terminal objects remain operational after they are mapped
  to the grid, but their construction catalog is not player-placeable yet.

## Contracts and migration

Schema 10 adds sparse floor cells, wall/door edges, stable placed objects,
derived rooms, local construction jobs, closures, and edit transactions to the
Phase 3 domain. UI gestures submit typed commands; presentation consumes only
immutable revisioned queries.

Schemas 1-9 migrate deterministically. Saves without initialized passenger
terminal state receive the furnished GA layout. Initialized Phase 3 saves
receive the authored passenger fixture expressed through the spatial layout and
preserve lifecycle, routes, passengers, bags, teams, tenant, flight, economy,
and reconciliation state.

## Visual and asset lock

VA-03 remains the primary reference. Two 1920 x 1080 target frames must be
approved before runtime asset generation:

1. starter GA terminal in construction mode;
2. grown regional terminal during passenger operations.

The runtime kit uses the existing 24 terminal sprites where suitable and adds
normalized floors, wall systems, glass, doors, roof pieces, construction
treatments, GA furniture/amenities, placement overlays, and terminal-editor
icons. OpenAI Image Gen owns composition targets; reviewed ComfyUI candidates
may supply simple seamless floor and roof surfaces; structural geometry,
overlays, and icons remain deterministic editable assets. Runtime references
must be serialized or constructor-visible and cooker-visible.

## Visual measurements

- Reference and target state: 1920 x 1080 at 100 percent UI scale.
- Persistent status occupies no more than 9 percent of height.
- Normal terminal tools and contextual inspector each target 15-17 percent of
  width; the continuous cutaway remains at least 60 percent of screen area.
- The one-meter grid is visible only in Build mode and becomes subordinate at
  operational zoom.
- Public floors use warm neutrals, secure/departure routes cyan, arrivals and
  reclaim purple, landside flow green, congestion amber, and accessible routes
  yellow pattern plus icon/text.
- At 175-200 percent scale the tool rail and inspector become compact drawers;
  typography does not shrink below the shared accessibility baseline.

## Work packages

1. **PI11-00 — visual gate:** generate and approve both target frames and their
   annotations.
2. **PI11-01 — asset kit:** normalize retained sprites, create the supplemental
   modular kit, review a contact sheet, record provenance, and import approved
   cooker-visible assets.
3. **PI11-02 — spatial domain:** add schema 10, terminal layout commands,
   validation, checksum, derived topology, fixtures, and migrations.
4. **PI11-03 — construction loop:** implement immediate spending, undo,
   demolition, local closures, worker jobs, and visible stage changes.
5. **PI11-04 — world integration:** implement pooled layout proxies, generated
   roof, seamless cutaway camera behavior, sorting, overlays, and responsive UI.
6. **PI11-05 — living terminal:** seed the furnished GA terminal, animate GA
   visitor journeys, and spatialize the Phase 3 passenger fixture.
7. **PI11-06 — hardening:** run focused/full automation, visual comparison,
   migration, package, dependency, asset-inventory, and owner-package checks.

## Completion gate

PI-11 completes only when both target frames and the asset contact sheet are
approved; the starter terminal is editable and visibly used; schemas 1-10
migrate and replay deterministically; the Phase 3 passenger fixture retains its
behavior; the supported UI-scale matrix passes; clean Development and Shipping
packages exercise the cooked terminal assets; and verification, provenance,
current state, and comparison records are current.

## Verification closeout

- Both OpenAI target frames were approved before bulk asset production.
- The deterministic modular kit retained 44 normalized 32-pixels-per-meter
  assets; its contact-sheet validation scored 96.167 with zero failed checks.
  ComfyUI surface candidates were rejected rather than imported because their
  seam/slab quality did not meet the visual standard.
- Schema 10, schemas 1-9 migration, terminal topology, costs, undo/refunds,
  demolition salvage, construction workers, pooled presentation, and revision
  gating are covered by focused tests. The final complete AMSim report contains
  81 discovered entries, zero failed, zero not-run, and zero in-process tests.
- Starter and grown runtime captures pass their deterministic journeys. The
  five-scale 1920 x 1080 matrix passes the starter editor, grown operations,
  and component gallery at 100-200 percent; compact scales retain a dominant
  world region with scrollable drawers.
- Clean Development and Shipping BuildCookRun archives pass. The packaged
  terminal proof renders the full spatial terminal, all 88 TerminalGrowth
  texture/sprite packages are in the cooked reference set, and packaged
  runtime/save-load/offline checks pass.
- Technical release-audit fields are clean: zero runtime string asset loads,
  forbidden runtime dependencies, required 3D assets, or line-limit
  violations. The aggregate Phase 7 release audit remains pending only because
  its separate final owner-acceptance field is intentionally still pending.

Continuing visual debt and intentional scope differences are recorded in
`terminal-growth-visual-validation/README.md`. They do not block this starter
vertical slice; the passenger placement catalog and upper floors remain future
iterations.
