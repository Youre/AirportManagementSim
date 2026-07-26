# PI-05: Phase 3 Terminal and Landside Implementation Plan

**Status:** Complete
**Owner:** Production and integration
**Opened:** 2026-07-26
**Closed:** 2026-07-26
**Roadmap phase:** Phase 3

## Purpose

Phase 3 adds the first complete passenger-airport vertical slice:

`build a terminal -> connect landside, pedestrian, controlled, gate, and baggage networks -> open the terminal -> schedule one passenger service -> receive arriving passengers and bags -> process departing passengers and bags -> board, reclaim, and leave with exact reconciliation`

This phase extends the accepted Phase 1.5 component language and Phase 2 living-airport simulation. It does not add international border processing, connections, rail, rental cars, cargo, or Phase 4 timetable breadth.

## Authoritative scope

Phase 3 integrates the Phase 3 portions of:

- PX-02 through PX-04;
- GS-01, GS-02, GS-04, GS-06 through GS-13, GS-15, and GS-16;
- CT-01 and CT-04;
- TS-01 through TS-06, TS-08, and TS-09;
- scenario `S05 Passenger departure and arrival`.

The gate requires one domestic passenger turnaround, exact passenger and bag totals, no sterile-area bypass, an accessible route, and a 2,000-visible/10,000-logical presentation fixture within the existing budgets.

## Player-visible definition

### Terminal construction and networks

The player can fund and complete a compact domestic terminal shell, then connect:

- an external road to curb, parking, taxi, and bus access;
- an entrance to check-in and bag drop;
- check-in to a staffed, accessible security checkpoint;
- the sterile departure lounge to Gate A1;
- the aircraft stand to the arrivals corridor and baggage reclaim;
- bag drop through screening, sorting, make-up, aircraft, infeed, and reclaim.

Opening fails with a short cause and corrective action until all paths, controlled boundaries, staff, and capacity rules pass. A normal door never creates an implicit sterile-area bypass.

### Passenger journey

The first service uses fictional Riverbend Connect and a project-authored passenger-aircraft presentation descriptor. Named passengers remain in stable parties with locally generated names, mobility profiles, arrival modes, bag counts, time confidence, and compact needs.

The selected party panel follows VA-03's hierarchy:

1. party identity and member count;
2. flight and punctuality;
3. current process step and progress;
4. compact needs with icon-plus-text meaning;
5. complete route and accessible-route status;
6. time confidence.

Arriving and departing cohorts move simultaneously where their paths permit. Queues have capacity, parties remain together through ordinary flow, the accessible passenger uses the accessible checkpoint and route, and a missing route produces an actionable wait state rather than deleting an entity.

### Baggage and security

Each checked bag has a stable identity, owning passenger, flight, screening state, route state, and final reconciliation result. Representative bag sprites make the two conveyor directions visible while the exact logical total remains authoritative.

Departure bags follow accepted, conveyor, screened, sorted, make-up, aircraft. Arrival bags follow aircraft, infeed, reclaim, collected. The flight cannot become departure-ready until boarded passengers and their known checked bags reconcile.

The domestic security boundary uses a continuous cyan shield-and-dash treatment plus text in the legend. Screening includes document check, queue, checkpoint, an accessible lane, and age-appropriate cleared/secondary states. No procedural harm or evasion detail is modeled.

### Landside and people

Cars, taxis, and a public bus connect the external road to curb and terminal entrance. Parking, curb, and bus capacity are explicit. Passenger counts entering and leaving the map reconcile with the terminal and aircraft populations.

Phase 3 adds passenger-service, security, baggage, and terminal-operations teams plus one passenger operator tenant. Teams remain role-based authoritative units; individually drawn staff are presentation only.

## VA-03 visual contract

The 1672 x 941 concept image is normalized to a 1920 x 1080 implementation capture at 100% UI scale and equivalent complete-terminal state.

The implementation must preserve:

- a world-dominant top-down terminal cutaway framed by deep navy chrome;
- a compact left terminal tool rail, large continuous terminal surface, right selected-party rail, and bottom overlay legend;
- warm floor fields, cool sterile zones, purple arrivals/baggage-reclaim language, green landside flow, cyan secure boundary, amber congestion, and yellow accessible route;
- clearly labeled entrance, check-in/bag drop, security, gate, arrivals corridor, baggage make-up, baggage reclaim, curb, parking, and bus stop;
- directional paths placed over the world and connected to the legend;
- rounded panels, generous grouping, off-white type, cyan selection, amber timing, and restrained coral critical state;
- named-party inspection with short scanable copy rather than a debug ledger;
- non-color route encoding through line pattern, arrow direction, shield/door/accessible symbols, and text.

Intentional specification-driven differences:

- the real Phase 3 fixture uses its own flight code, counts, times, prices, room dimensions, and throughput;
- the terminal is rendered with reproducible project-owned 2D presentation assets and pooled proxies, not the concept image or 3D geometry;
- only domestic processing is active; customs, immigration, connections, rental cars, and rail remain Phase 4;
- the normal layout uses the complete rail composition at 100-150%; 175-200% uses a compact action drawer and selected-party drawer so text is not shrunk.

## Architecture plan

### Simulation composition

`AMSimSimulation` gains `FPassengerTerminalSimulation`, composed by `FSimulation` beside the Phase 1 and Phase 2 domains. Phase 3 owns only terminal, passenger, bag, security, landside, passenger-staff, passenger-tenant, and passenger-flight records. Phase 1 remains the economy owner and Phase 2 remains the living-airport/airframe owner.

All player mutations use typed Phase 3 commands at fixed-step boundaries. The domain publishes causal events and an immutable revisioned `FPhase3QuerySnapshot`. Stable IDs, named deterministic fixture data, canonical iteration order, integer time and counts, and no per-entity Actor Tick remain mandatory.

### Persistence

The authoritative save contract increments to schema 4. Schema 4 stores the Phase 3 domain, its ID/event counters, terminal lifecycle, network topology, passenger parties and identities, bags, queues, security clearance, transport counts, staff, tenant, and flight state.

Schema 1-3 migrations create an empty Phase 3 domain without inventing passenger progress. Restore validation rejects duplicate IDs, broken ownership, invalid counts, passenger/bag/flight reference failures, impossible security transitions, a sterile bypass, a missing accessible route for a mobility-restricted passenger, and unsupported future versions. Actors, Widgets, route caches, proxy assignments, selected panel state, and overlay animation remain excluded.

### Presentation

`AMSimUI` adds a dedicated terminal presenter rather than expanding the near-limit root-screen source. It consumes only Phase 3 queries/state and submits typed commands. The root continues to own CommonUI input policy, save/load, time, and the persistent visible mouse.

The terminal presenter overlays the accepted root shell with VA-03's left-tool/center-world/right-inspector/bottom-legend composition. The Paper2D world presenter adds terminal rooms, controlled boundaries, networks, passenger clusters, representative bags, and landside vehicles as pooled presentation-only proxies.

Required production assets use serialized or constructor-time references and `/Game/Phase3` is explicitly cooked. Runtime string asset loading is prohibited.

## Work packages

### P3-00: Contract, visual measurement, and content lock

- Freeze Phase 3 stable IDs, fixture values, fictional operator, passenger name set, and `S05`.
- Record the VA-03 region proportions, palette roles, room labels, route encodings, party-panel hierarchy, density, and intentional differences.
- Create the Phase 3 architecture-impact and adversarial reviews.

**Exit:** every Phase 3 record, command, definition, visual surface, and acceptance assertion has an owner.

### P3-01: State contracts and schema 4

- Add Phase 3 IDs, records, commands, events, queries, validation, checksum, and fixture.
- Compose the domain through `FSimulation`.
- Serialize schema 4 and retain schema 1-3 migration coverage.

**Exit:** deterministic replay, round-trip, migration, future-schema, corruption, reference, bypass, and accessible-route tests pass.

### P3-02: Terminal, networks, security, and landside

- Implement staged terminal construction and opening.
- Add typed pedestrian, controlled, baggage, and landside connections.
- Add domestic security, accessible checkpoint, staff coverage, roads, curb, parking, taxi, and bus.

**Exit:** opening is blocked until every required connection is valid; no bypass exists; the accessible route succeeds.

### P3-03: Passenger, bag, flight, and reconciliation

- Add named parties, needs, mobility, transport choice, queues, and complete arrival/departure states.
- Add exact bag ownership, screening, sorting, make-up, aircraft, infeed, reclaim, and exception state.
- Gate departure readiness on passenger and bag reconciliation.

**Exit:** `S05` completes deterministically with exact regional, terminal, aircraft, and bag totals at every step and after save/load.

### P3-04: UI and Paper2D presentation

- Build the VA-03-aligned terminal composition from the accepted theme and new terminal templates.
- Add world labels, room/zone fields, directional overlays, secure boundary, accessible route, congestion treatment, named-party inspector, compact needs, and legend.
- Add pooled terminal, passenger, bag, staff, and landside proxies.

**Exit:** the complete state is usable at 100-200%, the cursor remains visible, statuses are non-color-only, and the equivalent-state VA-03 comparison has no P0-P2 mismatch.

### P3-05: Scale, package, and closeout

- Run focused and full automation, deterministic replay, schema/save matrix, `S05`, the 10,000-logical fixture, and the 2,000-visible proxy fixture.
- Capture the equivalent terminal state at all supported UI scales.
- Build clean Development and Shipping packages, exercise the packaged journey, enumerate every Phase 3 asset, and scan dependencies, sockets, runtime loads, and 3D candidates.
- Update traceability, current state, verification, provenance, visual review, and design QA.

**Exit:** every Phase 3 roadmap gate is evidenced, then the coherent change is committed and pushed.

## Completion gate

Phase 3 is complete only when:

- one domestic passenger turnaround completes through normal typed commands;
- every passenger and bag reconciles exactly at every authoritative step;
- terminal opening and passenger routing prove no security bypass;
- the mobility-restricted party completes an accessible route;
- curb, parking, taxi, and bus counts reconcile with terminal populations;
- save/load continues identically from construction, security queue, boarding, baggage, reclaim, and landside boundaries;
- the 10,000-logical and 2,000-visible fixtures meet deterministic and performance budgets;
- all Phase 3 commands are available through player-facing UI;
- VA-03 comparison and the 100-200% scale matrix pass;
- Shipping contains and exercises every required Phase 3 asset with zero forbidden editor/MCP/test dependency, socket, runtime string load, or required 3D match;
- planning, traceability, current state, verification, provenance, and review records are current;
- the completed Phase 3 change is committed and pushed.

The formal unassisted tester protocol and four-hour physical-tier soak remain Phase 7 gates.

## Closure evidence

Phase 3 passed on 2026-07-26. The normative closeout records are:

- [Phase 3 verification](../../../.ai/reviews/2026-07-26-phase-3-verification.md);
- [Phase 3 architecture-impact review](../../../.ai/reviews/2026-07-26-phase-3-architecture-impact.md);
- [Phase 3 adversarial review](../../../.ai/reviews/2026-07-26-phase-3-adversarial-review.md);
- [Phase 3 VA-03 visual review](phase-3-visual-review.md);
- [Phase 3 content provenance](../30-content-and-assets/phase-3-content-provenance.md);
- project-root `design-qa.md`;
- machine-readable local evidence at
  `AMSim/Saved/Phase3/pipeline-result.json`.
