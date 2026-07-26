# PI-04: Phase 2 Living General-Aviation Implementation Plan

**Status:** Complete
**Owner:** Production and integration
**Opened:** 2026-07-26
**Completed:** 2026-07-26
**Roadmap phase:** Phase 2

## Purpose

Phase 2 turns the accepted starter-airfield slice into a living general-aviation airport that can sustain overlapping operations for two weeks of simulated time:

`grow the starter airport -> host multiple persistent aircraft -> schedule recurring training and charter work -> dispatch staff and vehicles -> operate through changing weather and minor incidents -> expand land and facilities -> remain solvent and recoverable`

The visual foundation accepted in Phase 1.5 remains the presentation baseline. Phase 2 extends its templates, world proxies, and comparison loop; it does not replace them with a separate interface language.

## Authoritative scope

Phase 2 integrates the Phase 2 portions of:

- PX-01 through PX-04;
- GS-01, GS-03 through GS-07, GS-11, GS-12, and GS-15 through GS-18;
- CT-01 through CT-04;
- TS-01 through TS-06, TS-08, and TS-09;
- scenarios `S02 Sustained GA`, `S03 Flight school`, `S04 Charter`, `S09 Weather and deicing`, `S10 Basic incident`, `S11 Construction closure`, `S12 Recovery`, and `S15 Network-denied clean package`.

Terminal interiors, passengers, baggage, security, cargo, public transport, and special events remain later-phase work.

## Player-visible definition

### Living traffic

The player can manage multiple persistent airframes with stable identities, capabilities, liveries, visit history, service history, and current operational state. Phase 2 supplies at least four fictional 2D aircraft roles:

- basic trainer;
- touring piston;
- business turboprop;
- light business jet.

One-time, recurring, training, and charter offers expose duration, cadence, runway and stand requirements, service demand, expected income, cancellation terms, and compatibility reasons before acceptance. A seven-day timetable detects runway, stand, protected-buffer, staff, vehicle, closure, and weather conflicts. Rescheduling and cancellation use explicit commands and explained consequences.

### Airside operation

The airport supports:

- a declared active runway direction and visible runway-use reason;
- PAPI or VASI readiness appropriate to the installed runway;
- typed movement routes and reservations for multiple aircraft;
- hold-short behavior, congestion reasons, and bounded wait diagnostics;
- compatible stand assignment and towing between eligible positions;
- concurrent, dependency-aware turnaround tasks;
- persistent fuel, inspection, and towing vehicles with home depots, travel, occupancy, workload, and availability;
- deicing when weather and aircraft conditions require it.

ATC and service speech remain deterministic phrase intents. Captions carry the complete operational meaning and continue to work if speech is unavailable.

### People and tenants

At least four staff-role teams can be scheduled, assigned to zones, inspected for workload and morale, and reallocated without direct entity micromanagement. Coverage and queue effects are visible before they become failures.

The player can host three viable identities:

- general-aviation field;
- flight-school base;
- charter airport.

Each identity has a fictional tenant, transparent requirements, contract evidence, benefits, risks, and a specialization progress path. Selecting one path must not silently invalidate the others.

### Growth, weather, and recovery

The player can inspect and purchase a neighboring parcel, then complete a staged construction project with material delivery, worker access, closure effects, inspection, and save/load continuity.

A deterministic seeded forecast presents current conditions and upcoming operational categories. Weather changes affect runway selection, service demand, and schedule risk without becoming an arbitrary hidden penalty.

Phase 2 includes non-catastrophic operational incidents such as a disabled aircraft, fuel spill, or minor weather closure. Readiness, response assignment, containment, reopening, cost, and rating effects are explicit. Recovery offers remain bounded, labeled, and incapable of producing a permanent fail state.

The five rating components—passenger experience where applicable, airline or tenant satisfaction, staff welfare, safety/compliance, and operational reliability—publish current values, recent causes, and remedies. Objectives and achievements are evidence-based and never substitute for simulation state.

## Content policy

All Phase 2 operators, tenants, registrations, and liveries are fictional. The basic trainer may reuse the approved project-authored Phase 1 asset. Additional Phase 2 aircraft use deterministic editable 2D source art authored for this project until a separate CT-02 import and rights review approves any external master.

No file from `C:\Users\dave\Documents\Joes_Game\dist\assets` is copied by this plan. Any later import requires the approved manifest process: exact source, checksum, creator/license, reuse approval, cleanup, dimensions, 32 pixels-per-meter scale, pivot, heading variants, destination, and reviewer.

Required Phase 2 content has stable Primary Asset identities and review records for maps, parcels, aircraft, operators, tenants, facilities, vehicles, services, staff roles, weather profiles, incidents, objectives, achievements, presentation descriptors, and balance profiles. Missing required content is a development audit and cooking failure.

## Architecture plan

### Simulation composition

`AMSimSimulation` gains a focused Phase 2 domain composed by `FSimulation`. The Phase 1 domain remains the authoritative owner of the existing airport identity, base facilities, Credits, Airport Points, transaction history, and starter journey. Phase 2 owns only the new living-airport records and applies economy/progression effects through typed Phase 1 integration commands.

Phase 2 records include:

- persistent aircraft roster, visits, contracts, recurring services, and flight instances;
- runway configuration, approach-aid readiness, movement reservations, congestion, and towing;
- service tasks, dependency graphs, vehicle instances, depots, travel, and deicing;
- staff teams, shifts, zones, assignments, workload, and morale;
- tenants, requirements, leases, evidence, and specialization progress;
- owned and offered parcels, purchases, staged projects, deliveries, closures, and inspections;
- current and forecast weather observations with named deterministic streams;
- incidents, readiness, response tasks, containment, recovery, and reopening;
- rating components, causal contributions, objectives, achievements, unlocks, and recovery offers.

Every player mutation is a validated command applied at a fixed-step boundary. Domain events contain causal stable IDs. Immutable Phase 2 query snapshots contain only player-facing truth and presentation-ready derived values.

### Persistence

The first Phase 2 authoritative merge increments the save contract to schema 3. Schema 3 stores Phase 1 state plus all authoritative Phase 2 records, ID counters, named random-stream states, balance/content versions, and release-manifest hash.

Retained schema-1 and schema-2 fixtures migrate forward without inventing completed Phase 2 progress. Save validation rejects duplicate IDs, impossible lifecycle combinations, invalid reservations, missing authoritative references, corrupt count bounds, non-finite values, and unsupported future schemas. Actors, Widgets, presentation proxies, layout state, route caches, and audio remain excluded.

Required continuation boundaries include active construction, taxi and towing, concurrent turnaround, weather transition, deicing, and incident response.

### Unreal lifecycle and presentation

`AMSimGameplay` owns Phase 2 Primary Data Asset resolution, fixed-step lifecycle, save orchestration, deterministic fixtures, and immutable translation to Unreal-facing presentation state.

`AMSimUI` extends the accepted shell with reusable timetable, fleet, staff, service dispatch, tenant, parcel, weather, rating, objective, and incident components. C++ owns queries, commands, input policy, and refresh cadence. Widget composition and visual variants remain presentation concerns. Long text, focus, disabled state, color-independent status, and compact scaling continue to use the Phase 1.5 component rules.

Paper2D presentation adds pooled aircraft, vehicle, closure, service, weather, incident, construction, and selection proxies. It never owns a route, reservation, task outcome, position, or clock. Presentation refresh remains revision-gated with bounded clock/progress interpolation.

## Work packages

### P2-00: Contract, fixture, and content lock

- Freeze Phase 2 stable IDs, deterministic seeds, balance profile, and three specialization fixtures.
- Author the aircraft-role, tenant, staff-role, service-vehicle, weather, incident, and parcel matrices.
- Add CT-02/CT-04 content and provenance reviews.
- Add `S02`, `S03`, `S04`, `S09`, `S10`, `S11`, `S12`, and `S15` machine-readable fixtures.

**Exit:** every required definition and scenario has an owning specification, stable identity, validation rule, and expected result.

### P2-01: State contracts and schema 3

- Add focused Phase 2 IDs, records, commands, events, queries, validation, and checksums.
- Compose the new domain through `FSimulation`.
- Add the typed Phase 1 economy/progression integration boundary.
- Serialize schema 3 and retain schema-1/schema-2 migration fixtures.

**Exit:** deterministic round-trip, migration, corruption, future-schema, duplicate-ID, missing-reference, and boundary-continuation tests pass.

### P2-02: Fleet, contracts, timetable, runway, and movement

- Add the four-role persistent fleet and fictional operators.
- Implement one-time, recurring, training, and charter offers and instances.
- Validate seven-day scheduling, explicit buffers, closures, resources, and weather risk.
- Add active-runway reasoning, approach-aid readiness, multi-aircraft reservations, congestion diagnostics, and towing.

**Exit:** `S02`, `S03`, and `S04` can schedule and operate overlapping traffic deterministically with clear rejection/remedy paths.

### P2-03: Services, vehicles, staff, and zones

- Implement dependency-aware turnaround tasks and priority.
- Add persistent service vehicles, depots, dispatch, travel, occupancy, and failure recovery.
- Add staff-role teams, shifts, zone coverage, workload, and morale.
- Connect resource availability to timetable feasibility and live operations.

**Exit:** overlapping turnaround work neither double-books resources nor deadlocks, and player-facing queries explain every delay.

### P2-04: Tenants, land, economy, progression, and ratings

- Add GA, flight-school, and charter tenants with transparent requirements and evidence.
- Add neighboring land purchase and staged logistics/closure construction.
- Add itemized recurring revenue, wages, services, construction, incidents, recovery, and bounded assistance.
- Add five-component ratings, objectives, achievements, unlocks, and specialization progress.

**Exit:** each identity is viable and solvent for 14 simulated operating days from its approved fixture, and the recovery scenario returns to safe operation.

### P2-05: Weather, deicing, and basic incidents

- Add current and forecast conditions from named deterministic streams.
- Connect conditions to runway selection, schedule risk, operations, and deicing.
- Add readiness, response, containment, cleanup, reopening, and causal rating/economy effects for basic incidents.

**Exit:** `S09` and `S10` reproduce from seed, expose advance warning or a clear cause, and save/load through every response boundary.

### P2-06: UI and Paper2D extension

- Add management surfaces using Phase 1.5 theme tokens and component templates.
- Add world overlays and pooled 2D proxies for all Phase 2 operational states.
- Preserve a world-dominant viewport, persistent visible mouse, semantic input, captions, and focus restoration.
- Produce equivalent-state VA-05, VA-06, and VA-07 comparison evidence at supported UI scales.

**Exit:** all Phase 2 commands are usable without debug controls; visual inspection records the three highest-impact mismatches and any intentional differences.

### P2-07: Integrated hardening and release evidence

- Run focused, module, full automation, deterministic replay, rendered smoke, save/load, and recovery suites.
- Run each specialization fixture for 14 simulated operating days without insolvency, deadlock, invalid state, or unexplained resource conflict.
- Build clean Development and Shipping packages and exercise the complete paths offline.
- Verify required Phase 2 assets are cooked.
- Scan Shipping for editor, tests, MCP, Python, RemoteControl, Toolset, forbidden network, and runtime string asset-loading dependencies.
- Update traceability, verification logs, content provenance, and current state.

**Exit:** the Phase 2 gate is evidenced. The formal unassisted tester protocol and four-hour physical-tier soak remain Phase 7 gates.

## Automated acceptance

At minimum, automation proves:

- determinism across repeated and resumed runs;
- schema-1 and schema-2 migration to schema 3;
- strong-ID/reference validation and bounded deserialization;
- four aircraft roles and persistent visit/service history;
- recurring/training/charter offer and timetable conflict behavior;
- runway, approach-aid, reservation, congestion, and tow rules;
- service dependency ordering and vehicle/staff exclusivity;
- shift, zone, workload, and morale effects;
- tenant requirements and specialization evidence;
- parcel purchase, logistics, closures, inspection, and cancellation;
- itemized economy, five ratings, achievements, objectives, unlocks, and recovery;
- seeded forecast, runway/weather consequences, deicing, incident response, and reopening;
- revision-gated view-state mapping and presentation isolation;
- proxy pooling, sorting, heading/state, and cooker-visible references;
- save/load at every required transient boundary;
- 14-day solvency and liveness for all three identity fixtures;
- clean packaged/offline operation with no forbidden dependency.

## Completion gate

Phase 2 is complete only when:

- the GA, flight-school, and charter fixtures each complete 14 deterministic operating days while solvent;
- multi-aircraft scheduling, movement, turnaround, staff, vehicles, construction, weather, and incidents have no deadlock or unexplained state;
- every required transient save/load boundary matches uninterrupted continuation;
- all player mutations are available through the approved UI;
- Phase 2 visual comparison and component-scale review pass;
- clean Shipping contains and exercises every required asset;
- the editor/MCP dependency scan has zero Shipping matches;
- the short offline packaged smoke test passes;
- planning, traceability, current state, verification, and provenance records are current;
- the coherent Phase 2 change is committed and pushed.

The four-hour physical-tier soak and formal unassisted new-tester protocol remain deferred to Phase 7.

## Completion record

All Phase 2 work packages and completion gates passed on 2026-07-26. The
normative evidence is:

- [Phase 2 verification record](../../../.ai/reviews/2026-07-26-phase-2-verification.md);
- [Phase 2 architecture-impact review](../../../.ai/reviews/2026-07-26-phase-2-architecture-impact.md);
- [Phase 2 adversarial review](../../../.ai/reviews/2026-07-26-phase-2-adversarial-review.md);
- [Phase 2 visual review](phase-2-visual-review.md);
- generated machine-readable results at
  `AMSim/Saved/Phase2/pipeline-result.json`.

The verified Shipping container directly lists all 33 required Phase 2 assets.
The three specialization fixtures pass 14 deterministic operating days, and
the integrated rendered/package fixture reaches operating day 8 with 24
completed flights, operational expansion, and a resolved incident. Phase 3 is
not opened by this completion record.
