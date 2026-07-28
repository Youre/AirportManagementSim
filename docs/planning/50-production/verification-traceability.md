# PI-02: Verification and Requirements Traceability

**Status:** Approved baseline  
**Owner:** Requirements and verification  
**Last updated:** 2026-07-26

## Purpose

This document proves that the living concept in `ideas.txt` has an implementation owner and that each phase produces evidence. It is updated when a requirement, specification, or phase changes.

## Concept-to-specification matrix

| Concept area | Normative owners | First complete phase |
| --- | --- | ---: |
| Core idea, audience, fantasy, child learning posture | PX-01, PX-03, CT-04 | 1 |
| Guided sandbox, saves, no ending, progression | PX-01, GS-15, GS-20, TS-05 | 2 |
| Starting an airport | GS-01, GS-02, PX-04 | 1 |
| Growth and all viable specializations | GS-12, GS-15 | 5; Major in 6 |
| Land, construction workers, deliveries, affected closures | GS-01, GS-11 | 2 |
| Full terminal design | GS-08, GS-02, TS-03 | 3 |
| Visible baggage | GS-09 | 3 |
| Named passengers and compact needs | GS-08 | 3 |
| Security, passenger inspection, customs, immigration | GS-10 | 3 domestic; 4 international |
| Roads, parking, buses, taxis, rental cars, trains | GS-13 | 3; rail/rental in 4 |
| Real aircraft models, fictional operators, persistent visitors | GS-03, CT-02 | 2 onward |
| Flight operations, automated ATC, policy/override | GS-05, GS-06 | 1; full in 2 |
| Contracts, exact timetable slots and gates | GS-04 | 1; recurring in 4 |
| Turnaround and ground services | GS-07 | 1; full in 2 |
| Staff roles, teams, and zones | GS-11 | 2 |
| Tenants and satisfaction | GS-12, GS-16 | 2 |
| Simplified cargo | GS-14 | 5 |
| Weather, time, seasons, forecast, deicing | GS-17 | 2 |
| Emergency services, warned risk, abstract harm, recovery | GS-18, GS-15 | 2 basic; 4 serious |
| Rating, economy, objectives, achievements | GS-15, GS-16, GS-20 | 2 |
| Airshows and occasional events | GS-19 | 5 |
| 2D presentation and source aircraft art | CT-01, TS-06 | 0 onward |
| Voiced and captioned radio | CT-03 | 1 onward |
| Unreal, MCP, offline architecture | TS-01 through TS-09 | 0 |

## Universal constraint checks

Each release evidence set answers:

- **2D only:** Does the asset/package audit show no required 3D gameplay asset?
- **Single player/offline:** Does a network-denied package complete new/save/load?
- **Child-focused:** Did text, interaction, accessibility, and incident content pass review for ages 5–10 with basic reading?
- **Authentic terminology:** Are aviation terms used correctly and explained contextually?
- **Recoverable:** Can the fixture continue after no cash, major disruption, and the most severe supported incident?
- **Independent saves:** Is there no progression or entity sharing across slots?
- **Path viability:** Can every declared specialization meet its current phase band independently?
- **Deterministic:** Do repeated command streams and post-load continuations match checksums/events?

## Feature evidence record

For every specification implemented, the phase evidence links:

1. requirement/specification revision;
2. implementation issue or change;
3. automated test names and result artifact;
4. content-validation report;
5. save/migration fixture;
6. performance scenario and measurements;
7. packaged journey result;
8. human review notes for experience, accessibility, accuracy, or safety;
9. known limitations and owner.

Evidence uses exact engine version, build identifier, content-manifest hash, seed, map, scenario, hardware, and date. "Tested" without these identifiers is not sufficient.

## Phase 0 evidence record

**Result:** Complete and passed on 2026-07-25 local / 2026-07-26 UTC.

The normative closeout evidence is:

- [Phase 0 verification record](../../../.ai/reviews/2026-07-25-phase-0-verification.md);
- [Phase 0 adversarial review](../../../.ai/reviews/2026-07-25-phase-0-adversarial-review.md);
- [Phase 0 architecture-impact review](../../../.ai/reviews/2026-07-25-phase-0-architecture-impact.md);
- machine-readable local results at `AMSim/Saved/Phase0/pipeline-result.json` and `AMSim/Saved/Phase0/network-denied-result.json`.

| Evidence | Result |
| --- | --- |
| Engine/platform | Unreal Engine 5.8.0, Win64 |
| Automated contracts | 14 of 14 `AMSim.Phase0` tests passed |
| Deterministic proof | replay and post-load continuation passed; packaged checksum `4404817232840225737` |
| Save | async verified write, backup rotation, corrupt-current fallback, coalescing, and packaged continuity passed |
| Render proof | 1920 x 1080, 5,823 frames in 5.000017 s, 1164.596 average FPS, 1.488 ms p99 |
| Package boundary | Development and Shipping passed; zero forbidden Shipping file, manifest, or receipt matches |
| Offline boundary | elevated inbound/outbound firewall-denied journey passed; Shipping observed zero TCP sockets |
| Presentation | orthographic camera, CommonUI shell, Enhanced Input, and proxy reuse tests passed |
| Static content | definition/tag/dependency/cycle validator and 21-column CT-01 manifest template passed |
| Aircraft import | zero candidate PNG/SVG/aircraft assets copied into `AMSim/Content` |

The measurement host was a CORSAIR VENGEANCE i5200 with Windows 11 Pro build 26200, Intel Core Ultra 9 285K, approximately 63.4 GiB RAM, and NVIDIA GeForce RTX 5090 driver `32.0.16.1074`. It is Phase 0 measurement evidence, not the minimum or release-reference PC. A representative physical-tier soak remains due during Phase 7 release hardening.

Accepted residuals are metadata-only transitive editor plugin descriptors, the temporary use of `/Engine/Maps/Entry`, the Development-only local trace listener, and the absence of a pre-schema-1 migration fixture. None places editor code, network behavior, or unapproved aircraft content in Shipping. Phase 1 owns the project-map replacement; its first authoritative schema change owns a migration fixture.

## Phase 1 implementation evidence

[PI-03](phase-1-grass-airfield-implementation-plan.md) maps the grass-airfield slice to implementation work packages and evidence. Systems implementation verification passed on 2026-07-26. Visual and interaction polish remains active; formal long-soak and unassisted evidence is deferred to Phase 7 release hardening. The normative records are:

- [Phase 1 verification record](../../../.ai/reviews/2026-07-26-phase-1-verification.md);
- [Phase 1 implementation architecture-impact review](../../../.ai/reviews/2026-07-26-phase-1-implementation-architecture-impact.md);
- [Phase 1 implementation adversarial review](../../../.ai/reviews/2026-07-26-phase-1-implementation-adversarial-review.md);
- [Phase 1 closeout-hardening architecture-impact review](../../../.ai/reviews/2026-07-26-phase-1-closeout-architecture-impact.md);
- [Phase 1 closeout-hardening adversarial review](../../../.ai/reviews/2026-07-26-phase-1-closeout-adversarial-review.md);
- [Phase 1 player-facing content-lock architecture-impact review](../../../.ai/reviews/2026-07-26-phase-1-content-lock-architecture-impact.md);
- [Phase 1 player-facing content-lock adversarial review](../../../.ai/reviews/2026-07-26-phase-1-content-lock-adversarial-review.md);
- [Phase 1.5 visual-foundation plan](phase-1-5-visual-foundation-plan.md);
- [Phase 1.5 equivalent-state visual review](phase-1-5-visual-review.md);
- machine-readable local results at `AMSim/Saved/Phase1/pipeline-result.json`.

| Phase 1 claim | Current evidence |
| --- | --- |
| Fresh save reaches first complete visit | Headless and rendered/package `S01` pass; short owner testing continues; formal unassisted session deferred to Phase 7 |
| Starter plan is affordable and recoverable | 3,400 Credits, 32% contingency, ledger reconciliation, cancellation/refund, and zero-credit recovery pass |
| First traffic is timely and compatible | Immediate paused offer, exact five-minute slot, explicit Stand A1/buffers, and arrival timing pass |
| Construction is authoritative and persistent | Proposal/stage/delivery/team/inspection/open/close/cancel tests and save matrix pass |
| Aircraft operation is safe and deterministic | ATC, reservation, taxi, turnaround, readiness, replay, no-overlap, and intermediate-state catch-up assertions pass |
| UI explains cause, remedy, compatibility, and reward | Query-backed rendered matrix passes 100–200%; 175–200% uses compact objective/operations drawers; the corrective four-board component-language baseline was accepted on 2026-07-26 |
| Content is controlled | 20-asset catalog plus approved Phase 1.5 Cessna source master, checksum, fictional Riverbend livery, and 16 reviewed heading variants; later external aircraft still require separate review |
| Save evolution is safe | Schema-1 migration, schema-2 boundary continuation, corruption/backup/future validation pass |
| Performance meets a representative target | Development-host frame/simulation/save/memory budgets and short reference-profile harness pass; physical four-hour reference-tier run deferred to Phase 7 |
| Product remains strict 2D and offline | Shipping asset/dependency/socket scans and elevated firewall-denied S15 pass; Development journey/save-load succeeds and Shipping observes zero TCP sockets |

## Phase 2 implementation evidence

[PI-04](phase-2-living-ga-implementation-plan.md) maps the living
general-aviation slice to implementation work packages and evidence. Phase 2
passed on 2026-07-26. The normative records are:

- [Phase 2 verification record](../../../.ai/reviews/2026-07-26-phase-2-verification.md);
- [Phase 2 architecture-impact review](../../../.ai/reviews/2026-07-26-phase-2-architecture-impact.md);
- [Phase 2 adversarial review](../../../.ai/reviews/2026-07-26-phase-2-adversarial-review.md);
- [Phase 2 visual review](phase-2-visual-review.md);
- machine-readable local results at
  `AMSim/Saved/Phase2/pipeline-result.json`.

| Phase 2 claim | Evidence |
| --- | --- |
| Living traffic is persistent and concurrent | Seven persistent airframes across four roles, recurring flight instances, independent stands/reservations, visit/service history, and concurrent-flight assertions pass |
| Contracts and timetable have explicit lifecycle | Identity-compatible acceptance, protected rescheduling, active-operation cancellation conflict, cancellation cost, runway compatibility, and remedy paths pass |
| Services, vehicles, and staff are authoritative | Dependency-aware turnaround, four persistent vehicles, four teams, shift/zone/workload/morale state, exclusive-resource tests, dispatch, tow, and deicing pass |
| Three airport identities are viable | GA, flight school, and charter each complete 14 deterministic operating days while solvent and live |
| Growth and economy stay integrated | Parcel purchase, staged expansion, closures, inspection, itemized costs/rewards, ratings, achievements, and bounded recovery use the Phase 1 Credits/AP authority |
| Weather and incidents are recoverable | Seeded condition cycle, cold/wet deicing, disabled-aircraft cause/response/containment/reopening, save continuation, and rating/economy effects pass |
| Persistence evolves safely | Schema 3 round-trip plus retained schema-1/schema-2 migration, strong-ID/reference/lifecycle/count validation, and transient-boundary continuation pass |
| Presentation remains query-backed and 2D | Revision-gated view state, pooled Paper2D aircraft/vehicle/incident/expansion proxies, all player commands, visible mouse policy, and the 100–200% matrix pass |
| Content is controlled and cooked | Editor audit resolves 33 Phase 2 Primary Assets; direct Shipping IoStore inventory lists the same 33 names; zero external Phase 2 aircraft files and zero required 3D candidates |
| Package remains clean and offline | Development integrated smoke passes; Shipping launches cleanly with zero TCP sockets and zero forbidden editor/test/MCP/Python/RemoteControl/Toolset matches |

## Phase 3 implementation evidence

[PI-05](phase-3-terminal-landside-implementation-plan.md) maps the terminal and
landside slice to implementation work packages and evidence. Phase 3 passed on
2026-07-26. The normative records are:

- [Phase 3 verification record](../../../.ai/reviews/2026-07-26-phase-3-verification.md);
- [Phase 3 architecture-impact review](../../../.ai/reviews/2026-07-26-phase-3-architecture-impact.md);
- [Phase 3 adversarial review](../../../.ai/reviews/2026-07-26-phase-3-adversarial-review.md);
- [Phase 3 visual review](phase-3-visual-review.md);
- [Phase 3 content provenance](../30-content-and-assets/phase-3-content-provenance.md);
- machine-readable local results at
  `AMSim/Saved/Phase3/pipeline-result.json`.

| Phase 3 claim | Evidence |
| --- | --- |
| One complete domestic turnaround works | Typed construction, network, opening, scheduling, security, assistance, bag-recovery, and time commands complete RB 304 through S05 |
| Passenger and bag truth reconciles exactly | 52/52 passengers and 34/34 bags complete with exact aircraft, terminal, regional, and destination counts |
| Security cannot be bypassed | Controlled topology, opening validation, security queue/state transitions, rejection fixtures, and text-plus-boundary presentation pass |
| Accessibility is operational | Maya's mobility-restricted party uses the accessible entrance, route, checkpoint, and Gate A1 path; missing-route validation rejects invalid state |
| Landside populations reconcile | Private car, taxi, public bus, parking, curb, entrance, terminal, and completed counts remain explicit and exact |
| Persistence evolves safely | Schema 4 round-trip, schema 1-3 migration, future/corrupt/reference rejection, and construction/security/boarding/bag/reclaim/landside continuations pass |
| Scale contracts hold | 10,000 logical/2,000 visible proxy fixture, bounded pools, deterministic selection, sorting, and zero 8x backlog pass |
| Presentation is player-facing and concept-grounded | Dedicated query-backed presenter, pooled Paper2D world, visible mouse, VA-03 comparison, and 100-200% matrix pass with no P0-P2 visual defect |
| Content is controlled and cooked | Editor audit and direct Shipping IoStore inventory match all 34 project-authored Phase 3 Primary Assets; zero external Phase 3 aircraft files |
| Package remains clean and offline | Development packaged S05/save-load passes; Shipping launches with zero TCP sockets and zero forbidden, runtime string-load, or required 3D matches |

## Phase 4 implementation evidence

[PI-06](phase-4-regional-scheduled-airport-implementation-plan.md) maps the
regional scheduled-airport slice to implementation work packages and evidence.
Phase 4 passed on 2026-07-26. The normative records are:

- [Phase 4 verification record](../../../.ai/reviews/2026-07-26-phase-4-verification.md);
- [Phase 4 architecture-impact review](../../../.ai/reviews/2026-07-26-phase-4-architecture-impact.md);
- [Phase 4 adversarial review](../../../.ai/reviews/2026-07-26-phase-4-adversarial-review.md);
- [Phase 4 visual review](phase-4-visual-review.md);
- [Phase 4 content provenance](../30-content-and-assets/phase-4-content-provenance.md);
- machine-readable local results under `AMSim/Saved/Phase4`.

| Phase 4 claim | Evidence |
| --- | --- |
| Recurring regional schedule is authoritative | Three fictional contracts, 21 save-stable flights, exact five-minute slots, Gates A1/A2/R1, remote bus, locked horizon, early/late/weather state, and gate-warning confirmation pass |
| Connections and bags reconcile | 18 connect, 6 miss and rebook, and all 32 transfer bags complete with stable inbound/outbound ownership |
| International and landside flows reconcile | 96 passengers complete immigration/customs; 42 rental-car and 58 rail passengers retain open facility, tenant, capacity, frequency, and route evidence |
| Serious incident is warned and recoverable | Stored warning/cause/remedy/conditions/seed/decision precede activation; hold/divert/tow/protect/report/recover pass with affected-only closure and `No injuries` |
| Tenant loop completes | Completion, delay, connection, and safety evidence produce one accepted recurring-operator renewal |
| Persistence evolves safely | Schema 5 round-trip, schema 4 migration, future/reference/incident rejection, deterministic replay, and packaged save/load pass |
| Presentation is concept-grounded | VA-04/VA-06 combined boards, player-facing commands, Paper2D world layers, visible mouse, and 100-200% matrix pass |
| Content is controlled and cooked | Source and Shipping IoStore match all 34 project-authored Phase 4 assets; no external Phase 4 aircraft or generated runtime asset was retained |
| Package remains clean and offline | Packaged Development S06/S10 passes; Shipping launches with zero TCP sockets and zero forbidden, runtime string-load, or required 3D matches |

## Phase 5 implementation evidence

[PI-08](phase-5-cargo-specialization-breadth-implementation-plan.md) maps the
cargo and specialization-breadth slice to implementation and evidence. Phase 5
passed on 2026-07-27. The normative records are:

- [Phase 5 architecture-impact review](../../../.ai/reviews/2026-07-27-phase-5-architecture-impact.md);
- [Phase 5 adversarial review](../../../.ai/reviews/2026-07-27-phase-5-adversarial-review.md);
- [Phase 5 verification review](../../../.ai/reviews/2026-07-27-phase-5-verification.md);
- [Phase 5 visual validation](phase5-visual-validation/README.md);
- [Phase 5 content provenance](../30-content-and-assets/phase-5-content-provenance.md);
- machine-readable local results under `AMSim/Saved/Phase5`.

| Phase 5 claim | Evidence |
| --- | --- |
| Cargo is a complete operating loop | All four classes and three directions retain contract, shipment, security, storage, task, flight, exception, conservation, deadline, and economy evidence |
| Dispatch remains ordinary and authoritative | Quantity cargo work uses Phase 2 task dependencies, teams, vehicles, route/equipment constraints, and completion callbacks |
| Dedicated and belly freight interoperate safely | Cargo-only freighters require no passenger terminal; compatible belly freight links to Phase 4 flight IDs without corrupting passenger/bag reconciliation |
| Six paths independently reach Advanced | Exact AP/day/readiness/reliability plus path evidence passes for GA, School, Charter, Cargo, Passenger, and Mixed while unrelated signature facilities are absent |
| Provider and concession choices are real | Six offers expose commercial/operating terms, satisfaction, shared-resource requirements, and visible grace/recovery |
| All event families use ordinary systems | Eight shared-lifecycle events retain linked ordinary entities, preview, accept/decline, preparation, active, partial-success, cleanup, reward, cooldown, and persistence |
| Persistence evolves safely | Schema 6 round-trip, schema 1-5 migration, deterministic replay, cargo/tenant/event boundary continuation, and future/corrupt/reference rejection pass |
| Presentation is concept-grounded and 2D | Four Figma-equivalent frames, dedicated query-backed views, concise rounded cards, and pooled Paper2D cargo/event proxies use VA-01/03/04/05/06/07 |
| Content is controlled and cooked | Shipping IoStore contains all 44 definitions and four freighter presentation assets; no external aircraft or required 3D asset exists |
| Package remains clean and offline | Development and Shipping packages pass; Shipping remains responsive with zero TCP sockets and zero forbidden executable/module or runtime string-load matches |

## Visual reference mapping

| Reference | Owning surfaces | First comparison gate |
| --- | --- | ---: |
| [VA-01 Airport overview and HUD](../30-content-and-assets/concept-art/01-airport-overview-hud.png) | PX-01, PX-03, CT-01, TS-06 | Phase 1 |
| [VA-02 Build mode](../30-content-and-assets/concept-art/02-build-mode-runway-extension.png) | PX-04, GS-01, GS-02 | Phase 1 |
| [VA-03 Terminal flow](../30-content-and-assets/concept-art/03-terminal-passenger-flow.png) | GS-08, GS-09, GS-10, GS-13 | Phase 3 |
| [VA-04 Flight planning](../30-content-and-assets/concept-art/04-flight-planning-timetable.png) | GS-04, PX-03 | Phase 1, expanded Phase 4 |
| [VA-05 Turnaround](../30-content-and-assets/concept-art/05-aircraft-turnaround.png) | GS-07, GS-11, TS-06 | Phase 1, expanded Phase 2 |
| [VA-06 Weather and incident](../30-content-and-assets/concept-art/06-weather-incident-response.png) | GS-17, GS-18, CT-03 | Phase 2, expanded Phase 4 |
| [VA-07 Progression](../30-content-and-assets/concept-art/07-progression-specializations.png) | PX-01, GS-12, GS-15 | Phase 2, completed Phase 6 |

At each comparison gate, evidence includes the reference image, equivalent implementation capture, required similarities, intentional differences, and any discrepancy where the written specification overruled generated art.

CT-05 now supplies stable visible-content IDs for every mapped reference. Phase
4.5 must close all `Absent`, `Text-only`, and `Schematic` rows before a visual
comparison can be described as concept-content complete. Layout, scale,
package, and interaction passes remain separate evidence and cannot substitute
for content completeness.

## Required scenario suite

Stable end-to-end scenarios are:

- `S01 Starter grass airfield`
- `S02 Sustained GA`
- `S03 Flight school`
- `S04 Charter`
- `S05 Passenger departure and arrival`
- `S06 Passenger connection and baggage transfer`
- `S07 Cargo outbound/inbound/transfer`
- `S08 Mixed airport congestion`
- `S09 Weather deterioration and deicing`
- `S10 Serious warned-risk incident and recovery`
- `S11 Construction with affected-only closure`
- `S12 Empty-cash recovery`
- `S13 Maximum supported population`
- `S14 Save migration matrix`
- `S15 Network-denied clean package`

Each scenario has a stable seed and machine-readable assertions. Human review supplements, not replaces, deterministic assertions.

## Change and contradiction control

If implementation reveals an omission, update the owning specification and this matrix before accepting code. If `ideas.txt` changes product intent, affected specifications return to Review until reconciled. If an initial tuning value changes but all documented outcomes remain true, update GS-20 and evidence without reopening unrelated behavior.

No phase closes with an unexplained missing concept row, broken document link, failed required scenario, or unresolved contradiction between a feature owner and the integration roadmap.
