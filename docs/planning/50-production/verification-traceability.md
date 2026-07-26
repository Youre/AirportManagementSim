# PI-02: Verification and Requirements Traceability

**Status:** Approved baseline  
**Owner:** Requirements and verification  
**Last updated:** 2026-07-25

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

The measurement host was a CORSAIR VENGEANCE i5200 with Windows 11 Pro build 26200, Intel Core Ultra 9 285K, approximately 63.4 GiB RAM, and NVIDIA GeForce RTX 5090 driver `32.0.16.1074`. It is Phase 0 measurement evidence, not the minimum or release-reference PC. A representative reference tier remains due before Phase 1 closes.

Accepted residuals are metadata-only transitive editor plugin descriptors, the temporary use of `/Engine/Maps/Entry`, the Development-only local trace listener, and the absence of a pre-schema-1 migration fixture. None places editor code, network behavior, or unapproved aircraft content in Shipping. Phase 1 owns the project-map replacement; its first authoritative schema change owns a migration fixture.

## Phase 1 implementation evidence

[PI-03](phase-1-grass-airfield-implementation-plan.md) maps the grass-airfield slice to implementation work packages and evidence. Implementation verification passed on 2026-07-26; the formal phase gate remains open for external evidence. The normative records are:

- [Phase 1 verification record](../../../.ai/reviews/2026-07-26-phase-1-verification.md);
- [Phase 1 implementation architecture-impact review](../../../.ai/reviews/2026-07-26-phase-1-implementation-architecture-impact.md);
- [Phase 1 implementation adversarial review](../../../.ai/reviews/2026-07-26-phase-1-implementation-adversarial-review.md);
- machine-readable local results at `AMSim/Saved/Phase1/pipeline-result.json`.

| Phase 1 claim | Current evidence |
| --- | --- |
| Fresh save reaches first complete visit | Headless and rendered/package `S01` pass; unassisted tester pending |
| Starter plan is affordable and recoverable | 3,400 Credits, 32% contingency, ledger reconciliation, cancellation/refund, and zero-credit recovery pass |
| First traffic is timely and compatible | Immediate paused offer, exact five-minute slot, explicit Stand A1/buffers, and arrival timing pass |
| Construction is authoritative and persistent | Proposal/stage/delivery/team/inspection/open/close/cancel tests and save matrix pass |
| Aircraft operation is safe and deterministic | ATC, reservation, taxi, turnaround, readiness, replay, and no-overlap assertions pass |
| UI explains cause, remedy, compatibility, and reward | Query-backed rendered matrix passes 100–200%; new-tester comprehension pending |
| Content is controlled | 20-asset catalog, internal 2D provenance, and zero copied external aircraft pass |
| Save evolution is safe | Schema-1 migration, schema-2 boundary continuation, corruption/backup/future validation pass |
| Performance meets a representative target | Development-host budgets pass; physical reference-tier run pending |
| Product remains strict 2D and offline | Shipping asset/dependency/socket scans pass; elevated firewall-denied S15 pending |

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
