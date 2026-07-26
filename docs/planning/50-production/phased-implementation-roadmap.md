# PI-01: Phased Implementation and Integration Roadmap

**Status:** Approved baseline  
**Owner:** Production and integration  
**Last updated:** 2026-07-26

## Purpose

This roadmap defines the order in which approved feature specifications become integrated, playable software. Phases deliver vertical slices, not disconnected system piles. No phase is complete until its gameplay journey, automated evidence, save compatibility, performance check, and packaged Windows smoke test pass.

## Dependency backbone

`Foundation -> Grass airfield -> Living GA airfield -> Passenger terminal -> Regional schedules -> Specializations -> Major airport -> Release hardening`

Simulation state is integrated before presentation for each domain, then commands, queries, UI, content, save migration, tests, and performance evidence are added in that same phase. Temporary debug presentation is acceptable inside a phase but cannot satisfy its exit gate.

## Phase 0: Technical foundation

**Status:** Complete — verification passed 2026-07-25 local / 2026-07-26 UTC. See [PI-02](verification-traceability.md#phase-0-evidence-record).

**Goal:** A clean 2D Unreal baseline that can deterministically run, inspect, save, and test an empty airport.

Deliver:

- TS-01 module boundaries and subsystem lifetimes;
- TS-02 clock, IDs, command/event/query contracts, random streams, checksum/replay fixture;
- TS-03 chunks and base occupancy;
- TS-04 assets/registries/tags and validator skeleton;
- TS-05 empty snapshot, slot metadata, async write, backup;
- TS-06 orthographic Paper 2D camera, CommonUI shell, input and proxy proof;
- TS-07 editor-only MCP setup/audit tools;
- TS-08 automation/package pipeline and recorded reference hardware;
- TS-09 network-denied Windows smoke test;
- CT-01 import manifest/template, without copying aircraft assets from `C:\Users\dave\Documents\Joes_Game\dist\assets`.

**Gate:** identical empty-world replay checksums; save/load continuity; 60 FPS proof scene; clean package; editor/MCP absent from Shipping.

## Phase 1: Grass-airfield vertical slice

**Status:** Complete through the accepted Phase 1.5 visual foundation. Long-soak and formal unassisted acceptance are deferred to Phase 7. See [PI-03](phase-1-grass-airfield-implementation-plan.md), [PI-03A](phase-1-5-visual-foundation-plan.md), and the [verification record](../../../.ai/reviews/2026-07-26-phase-1-verification.md).

**Goal:** Starting from a parcel, build and operate one complete light-aircraft visit.

Integrate PX-01, PX-02, PX-03, PX-04; GS-01/02 construction and validation; one GS-03 aircraft; one GS-04 offer; basic GS-05 runway/automated ATC; GS-06 taxi/stand; minimal GS-07 inspection/fueling; GS-11 construction and ramp teams; starter GS-15 economy; basic GS-16 feedback; clear weather/daylight from GS-17; CT-02/03/04 minimum content.

Journey: select temperate map -> build grass runway/taxi/stand/access -> number/open runway -> accept and slot a GA flight -> hear/read calls -> land/taxi/service/depart -> receive reward -> save/load.

**Development gate:** the complete journey, first-flight target, recoverable starting budget, deterministic/save/package/offline boundaries, persistent visible mouse interaction, and equivalent-state visual review pass. Short hands-on playtests inform iteration throughout development; the formal unassisted protocol and four-hour physical-tier soak are retained for Phase 7 rather than blocking Phase 2.

## Phase 2: Living general-aviation airport

**Status:** Complete — verified on 2026-07-26. See [PI-04](phase-2-living-ga-implementation-plan.md) and the [verification record](../../../.ai/reviews/2026-07-26-phase-2-verification.md).

Phase 2 begins after **Phase 1.5: Visual foundation and validation loop** closes. Phase 1.5 replaces the proof HUD/map with reusable CommonUI/UMG regions and a real Paper2D presentation, validates VA-01/02/04/05 at supported UI scales, and re-proves the clean packaged Phase 1 journey. See [PI-03A](phase-1-5-visual-foundation-plan.md).

**Goal:** Sustained operations with multiple aircraft, staff, construction logistics, changing weather, and GA/flight-school/charter identity.

Add persistent airframes; recurring and training flights; runway configuration/PAPI or VASI; surface congestion/towing; full service dispatch and vehicles; team zones/workload; GA, flight-school, charter tenants; land purchases; weather forecast and deicing; ratings, objectives, achievements, recovery assistance; basic emergency readiness and non-catastrophic incidents.

**Gate:** 14 simulated operating days remain deterministic and solvent for GA, flight school, and charter fixtures; save/load works during projects, taxiing, turnaround, weather change, and incident response.

## Phase 3: Terminal and landside vertical slice

**Status:** Not opened.

**Goal:** One complete departing and arriving passenger flight through a player-built terminal.

Add full terminal interiors and pedestrian routing; named passengers/parties/needs; check-in, queues, boarding, reclaim; visible baggage network; security and domestic controlled zones; roads, curb, parking, taxi, bus; passenger staff and tenants; appropriate audio/UI overlays.

Journey: build terminal -> connect landside/sterile/gate/baggage routes -> schedule flight -> passengers arrive/check bags/screen/wait/board -> turnaround -> arrivals reclaim/leave.

**Gate:** passenger and bag reconciliation is exact; no security bypass; accessible route succeeds; 2,000 visible/10,000 logical presentation fixture meets budgets.

## Phase 4: Regional scheduled airport

**Goal:** A multi-day timetable with several operators, connections, international processing, and disruption recovery.

Add recurring contracts, seven-day timetable and locked horizon; regional/narrow-body roster; contact/remote gates and buses; transfers; customs/immigration; rental cars and rail; tenant renewals; richer forecasts/seasons; runway approach limits; serious incident traceability/reporting.

**Gate:** seven-day passenger fixture handles early/late flights, gate changes, weather, missed connections, baggage transfer, and an incident while remaining recoverable and deterministic.

## Phase 5: Cargo and specialization breadth

**Goal:** Every promised specialization reaches a credible Advanced loop, and mixed airports have meaningful shared-resource choices.

Add cargo contracts/classes/warehouses; belly freight; advanced flight school, charter, GA and passenger capability; service providers/concessions; specialization-specific objectives/achievements; airshow, fly-in, cargo surge, and other special events.

**Gate:** automated and human playthroughs prove GA, flight school, charter, cargo, passenger, and mixed paths can each reach Advanced without building another path's signature facility.

## Phase 6: Major-airport capability

**Goal:** Complete the 10–15 hour progression target and large-scale systems.

Add large aircraft only after scale/art validation; parallel runway numbering/configuration; high-capacity terminals, baggage, cargo, stands, service fleets, transport, and staff; Major unlocks for every path; performance/aggregation work at maximum targets; complete recovery after severe aircraft-loss scenarios.

**Gate:** all six Major-path simulations pass solvency, capability, save, determinism, and performance; serious incidents remain warned, abstract, and recoverable.

## Phase 7: Content, accessibility, and release hardening

**Goal:** A shippable offline Windows game rather than a technically complete prototype.

Complete curated maps, approved aircraft/operator catalog, guidance/help, voiced/captioned radio coverage, visual/audio polish, accessibility review, balance playtests, rights/provenance, migration matrix, long soak, clean-machine package, security/offline audit, and child-safety content review.

Run the retained physical reference-tier four-hour soak and formal unassisted tester/audio/caption/comprehension protocol against a release-candidate package identity.

**Gate:** zero release-blocking issues; all traceability rows verified; package runs with network denied; every supported old save migrates; no 3D or unapproved source asset ships.

## Cross-phase integration rules

- A feature enters a phase only with its data, state, commands, events, queries, UI/feedback, save representation, tests, and diagnostics.
- Each UI/presentation gate captures the implemented state corresponding to the mapped [concept-art reference](../30-content-and-assets/concept-art/README.md), records intentional differences, and validates against the written specification rather than visual similarity alone.
- Later phases extend stable interfaces; they do not bypass earlier contracts.
- Save schema increments with migrations whenever authoritative representation changes.
- Content can be placeholder only inside a phase; exit evidence uses rights-cleared or explicitly internal-test assets.
- Performance is measured every phase at the largest then-supported scenario.
- A failed exit gate keeps the phase open; scope may be reduced only by revising the owning approved specification.
