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
