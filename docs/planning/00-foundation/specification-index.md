# Airport Management Sim Specification Index

**Status:** Approved  
**Authority:** Normative index for implementation planning  
**Concept source:** `ideas.txt`  
**Engine baseline:** Unreal Engine 5.8, Windows desktop, offline single-player  
**Last updated:** 2026-07-27

## Purpose

This index defines the approved specification set used to implement Airport Management Sim. `ideas.txt` remains the source of product intent. The approved specifications listed here are normative for behavior, interfaces, initial tuning, validation, and implementation order.

If a specification deliberately refines the concept, the specification controls implementation and must identify the refinement. The phased integration roadmap controls sequencing, not feature behavior. When two feature specifications conflict, the owning specification named in the dependency matrix controls its domain; unresolved cross-domain conflicts block implementation until this index records a decision.

## Universal specification rules

Every gameplay system must:

- run entirely offline;
- preserve a single-player, independent-save model;
- use deterministic simulation state with stable identifiers;
- expose player mutations as validated commands applied at simulation-step boundaries;
- publish immutable query data and domain events for UI and presentation;
- support pause and accelerated time;
- serialize all authoritative state through the versioned save contract;
- use data-driven tuning rather than hard-coded content values;
- provide plain, authentic aviation terminology suitable for children aged 5–10 with basic reading ability;
- remain recoverable after mistakes, financial shortage, and serious incidents;
- avoid per-entity Unreal ticking for high-volume simulation;
- include automation-level acceptance tests before its integration phase can close.

## Status model

All documents in this index are approved implementation baselines. Numeric defaults are initial tunable values, not promises that balancing will never change. A behavior change requires a specification revision and decision-log entry. A tuning-only change may update the owning data table without changing the specification if it stays within the documented outcome ranges.

## Player-experience specifications

| ID | Document | Ownership |
| --- | --- | --- |
| PX-01 | [Player Experience and Discovery](../10-player-experience/player-experience-and-discovery.md) | New game, guidance, achievements, recovery posture |
| PX-02 | [Camera, Controls, and Time](../10-player-experience/camera-controls-and-time.md) | Input, selection, camera, pause and speed |
| PX-03 | [Interface, Information, and Accessibility](../10-player-experience/interface-information-accessibility.md) | HUD, panels, overlays, alerts, readability |
| PX-04 | [Build Mode and Operational Feedback](../10-player-experience/build-mode-and-feedback.md) | Construction interaction, previews, validation, work zones |

## Gameplay-system specifications

| ID | Document | Ownership |
| --- | --- | --- |
| GS-01 | [World, Maps, Land, and Construction](../20-game-systems/world-maps-land-construction.md) | Curated maps, parcels, projects, workers |
| GS-02 | [Networks, Connectivity, and Facility Validation](../20-game-systems/networks-connectivity-validation.md) | Grid networks, ports, zones, connection diagnostics |
| GS-03 | [Aircraft Types and Persistent Airframes](../20-game-systems/aircraft-types-and-airframes.md) | Aircraft capabilities, tail numbers, visit history |
| GS-04 | [Flight Demand, Contracts, and Timetables](../20-game-systems/flight-contracts-and-timetables.md) | Offers, slots, gates, schedule feasibility |
| GS-05 | [Airspace, Runways, and ATC](../20-game-systems/airspace-runways-atc.md) | Patterns, runway use, clearances, sequencing |
| GS-06 | [Taxi, Ground Movement, Stands, and Gates](../20-game-systems/taxi-ground-movement-stands.md) | Surface routing, reservations, compatibility |
| GS-07 | [Turnaround and Ground Services](../20-game-systems/turnaround-and-ground-services.md) | Service tasks, dispatch, vehicles, priorities |
| GS-08 | [Terminal Interiors and Passenger Flow](../20-game-systems/terminal-interiors-passengers.md) | Rooms, passenger state, needs, movement |
| GS-09 | [Baggage System](../20-game-systems/baggage-system.md) | Bags, belts, screening, make-up, reclaim |
| GS-10 | [Security, Customs, and Immigration](../20-game-systems/security-customs-immigration.md) | Secure zones, processing, border flows |
| GS-11 | [Staff Teams, Zones, and Coverage](../20-game-systems/staff-teams-zones.md) | Role teams, assignments, workload |
| GS-12 | [Tenants and Airport Specializations](../20-game-systems/tenants-and-specializations.md) | Operators, requirements, viable endgames |
| GS-13 | [Ground Access and Transport](../20-game-systems/ground-access-transport.md) | Roads, parking, taxis, rental, bus, rail |
| GS-14 | [Cargo Operations](../20-game-systems/cargo-operations.md) | Cargo contracts, warehouses, freight flow |
| GS-15 | [Economy, Rewards, Progression, and Achievements](../20-game-systems/economy-progression-achievements.md) | Currencies, unlocks, recovery, milestones |
| GS-16 | [Ratings and Satisfaction](../20-game-systems/ratings-and-satisfaction.md) | Passenger, tenant, operational rating |
| GS-17 | [Weather, Calendar, and Operating Conditions](../20-game-systems/weather-calendar-operations.md) | Forecasts, METAR, seasons, operating categories |
| GS-18 | [Emergency Readiness, Incidents, and Recovery](../20-game-systems/emergencies-incidents-recovery.md) | Risks, casualties, rescue, closures, recovery |
| GS-19 | [Special Events](../20-game-systems/special-events.md) | Authored airshows, fly-ins, surges |
| GS-20 | [Initial Balance Defaults](../20-game-systems/initial-balance-defaults.md) | Central starting values and outcome targets |

## Content and presentation specifications

| ID | Document | Ownership |
| --- | --- | --- |
| CT-01 | [2D Visual Direction and Asset Pipeline](../30-content-and-assets/2d-visual-asset-pipeline.md) | Sprite rules, import, scale, animation, variants |
| CT-02 | [Aircraft and Operator Content Catalog](../30-content-and-assets/aircraft-operator-content.md) | Initial aircraft roster, fictional operators, liveries |
| CT-03 | [Radio, Audio, and Offline Speech](../30-content-and-assets/radio-audio-speech.md) | Phrase generation, local TTS, captions, caching |
| CT-04 | [Maps, Brands, Text, and Content Authoring](../30-content-and-assets/maps-brands-content-authoring.md) | Curated content, localization-ready text, validation |
| CT-05 | [Visual Content Requirements](../30-content-and-assets/visual-content-requirements.md) | Concept-derived visible content, composed surfaces, world/UI linkage, visual acceptance |

The [Concept Art Reference Set](../30-content-and-assets/concept-art/README.md) is approved supporting evidence for visual validation. It is subordinate to the written specifications and is not a source of numerical gameplay rules or shippable assets.

## Technical specifications

| ID | Document | Ownership |
| --- | --- | --- |
| TS-01 | [Runtime Architecture and Module Boundaries](../40-technical-design/runtime-architecture-modules.md) | C++ modules, Blueprint boundary, service lifetimes |
| TS-02 | [Deterministic Simulation and Entity Model](../40-technical-design/deterministic-simulation-entities.md) | Fixed step, IDs, commands, events, queries |
| TS-03 | [Spatial Grid, Routing, and Reservations](../40-technical-design/spatial-grid-routing.md) | Chunks, graphs, pathfinding, occupancy |
| TS-04 | [Data Assets, Registries, and Gameplay Tags](../40-technical-design/data-assets-registries-tags.md) | Static content schemas and validation |
| TS-05 | [Save, Migration, Backup, and Recovery](../40-technical-design/save-migration-recovery.md) | Versioned snapshots and compatibility |
| TS-06 | [2D Rendering, UI, and Presentation Proxies](../40-technical-design/rendering-ui-presentation.md) | Paper 2D, CommonUI, pooling, interpolation |
| TS-07 | [Unreal MCP and Editor Automation](../40-technical-design/unreal-mcp-editor-automation.md) | Editor-only tools, import, inspection, safety |
| TS-08 | [Testing, Performance, Diagnostics, and Builds](../40-technical-design/testing-performance-builds.md) | Automation tiers, budgets, logs, packaging |
| TS-09 | [Offline Services, Platform, and Security](../40-technical-design/offline-platform-security.md) | No-backend posture, local files, privacy, Windows |

## Production and integration specifications

| ID | Document | Ownership |
| --- | --- | --- |
| PI-01 | [Phased Implementation and Integration Roadmap](../50-production/phased-implementation-roadmap.md) | Phase order, dependencies, gates, vertical slices |
| PI-02 | [Verification and Requirements Traceability](../50-production/verification-traceability.md) | Concept coverage, test ownership, exit evidence |
| PI-03 | [Phase 1 Grass-Airfield Implementation Plan](../50-production/phase-1-grass-airfield-implementation-plan.md) | Phase 1 scope, architecture, work packages, and exit evidence |
| PI-04 | [Phase 2 Living General-Aviation Implementation Plan](../50-production/phase-2-living-ga-implementation-plan.md) | Phase 2 scope, architecture, work packages, and exit evidence |
| PI-05 | [Phase 3 Terminal and Landside Implementation Plan](../50-production/phase-3-terminal-landside-implementation-plan.md) | Phase 3 terminal, passenger, baggage, security, landside, presentation, and exit evidence |
| PI-06 | [Phase 4 Regional Scheduled Airport Implementation Plan](../50-production/phase-4-regional-scheduled-airport-implementation-plan.md) | Phase 4 recurring timetable, connections, border, disruption, presentation, and exit evidence |
| PI-07 | [Phase 4.5 Visual Content Remediation](../50-production/phase-4-5-visual-content-remediation-plan.md) | Cross-phase implementation of CT-05 before Phase 5 opens |
| PI-08 | [Phase 5 Cargo and Specialization Breadth](../50-production/phase-5-cargo-specialization-breadth-implementation-plan.md) | Cargo, independent Advanced paths, providers, concessions, events, schema 6, presentation, and exit evidence |
| PI-09 | [Phase 6 Major-Airport Capability](../50-production/phase-6-major-airport-capability-implementation-plan.md) | Major paths, parallel runways, wide-body operations, serious incidents, scale, schema 7, and exit evidence |
| PI-10 | [Phase 7 Content, Accessibility, and Release Hardening](../50-production/phase-7-release-hardening-implementation-plan.md) | Guidance, accessibility, offline radio, release content, schema 8, packaging, and final owner acceptance |

## Shared identifiers and interfaces

All authoritative entities use opaque, save-stable identifiers. Required ID domains are:

- `FAirportId`
- `FSimEntityId`
- `FAircraftTypeId`
- `FAircraftInstanceId`
- `FFlightId`
- `FContractId`
- `FTenantId`
- `FPassengerId`
- `FStaffTeamId`
- `FVehicleId`
- `FBagId`
- `FFacilityId`
- `FNetworkNodeId`
- `FConstructionProjectId`
- `FEventInstanceId`

IDs are never derived from array indices, display names, Actor names, or memory addresses. Static content uses Primary Asset IDs and gameplay tags; save instances use generated stable IDs.

The shared runtime boundaries are:

- commands: requested mutations with validation results;
- events: immutable facts emitted after accepted changes;
- query snapshots: read-only data optimized for UI and presentation;
- definitions: read-only content from data assets and registries;
- save snapshots: versioned authoritative state only.

Unreal Actors, Widgets, Sprites, audio components, and animations are presentation objects. They cannot be the sole owners of gameplay state.

## Change control

A specification revision must update:

1. the owning document;
2. affected cross-references;
3. the traceability matrix;
4. the phased roadmap if dependencies or sequencing change;
5. `.ai/context/decision-log.md` when product or architecture behavior changes.

Implementation may not silently invent missing behavior. If a genuine omission is discovered, update the specification before merging the implementation.
