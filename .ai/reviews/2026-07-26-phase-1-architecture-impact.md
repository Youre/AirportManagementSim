# Phase 1 Architecture-Impact Review

**Status:** Approved for implementation
**Date:** 2026-07-26
**Scope:** PI-03 Phase 1 grass-airfield vertical slice

## Outcome and boundaries

Phase 1 extends the Phase 0 empty-airport foundation into one deterministic, saveable, inspectable light-aircraft visit. It adds authoritative construction, economy, scheduling, aircraft movement, minimal service, guidance, and feedback state while retaining 2D/offline presentation and the existing module direction.

It does not authorize passenger/cargo systems, multiple-aircraft scale, changing weather, incidents, 3D gameplay assets, external services, runtime editor tooling, or unreviewed aircraft imports.

## Architecture decision

The existing six-module boundary remains sufficient:

- `AMSimSimulation` owns all Phase 1 records, rules, typed commands/events/queries, checksums, and snapshot state.
- `AMSimGameplay` owns Unreal lifecycle, data-asset/catalog resolution, project-map entry, stepping/backlog coordination, and save orchestration.
- `AMSimUI` owns CommonUI/UMG composition, semantic input adapters, captions/audio presentation, and pooled 2D proxies.
- `AMSim` remains bootstrap.
- `AMSimEditor` owns content/import/map validation and idempotent authoring tools.
- `AMSimTests` owns unit through package-facing fixtures and cannot enter Shipping.

No new runtime module or third-party runtime plugin is justified for Phase 1.

## State ownership

Authoritative state added in Phase 1 includes the airport identity, map/calendar/weather, parcel ownership, facilities, construction projects, networks, reservations, teams, economy ledger, objectives/help, rating reasons, offers/contracts/flights, airframes, ATC/movement, turnaround tasks, phrase intents, and relevant policies.

Actors, Paper 2D components, Widgets, audio clips, animations, routes/caches, and proxy handles remain derived or transient. A presentation callback cannot complete a project, move an aircraft into a new state, complete a service task, recognize revenue, or award progression.

## Contract evolution

The Phase 0 generic command/event records are intentionally a bootstrap surface. Phase 1 replaces broad generic branching with typed payloads, structured validation, explicit warning confirmation, canonical event causality, and domain query views.

Strong ID domains are added only where the slice uses them. Static content remains keyed by stable content IDs. Display names, UObject paths, Actors, array positions, and visual asset names cannot become authoritative keys.

Compatibility evaluators use typed length, surface, clearance, stand, service, and weather fields. Sprite dimensions and display labels cannot determine operational compatibility.

## Persistence and migration

The first authoritative Phase 1 state merge increments the save snapshot to schema 2. A pure schema-1-to-schema-2 migration preserves Phase 0 seed, time, revision, entities, and counters while initializing an unstarted Phase 1 airport state.

The save captures all new authoritative records, counters, streams, content IDs, rules version, and release-manifest hash. It continues to reject oversized, corrupt, appended, invalid-ID, missing-required-content, and future-schema input before activation.

Rollback retains immutable schema-1 fixtures and never overwrites the original source save during migration.

## Spatial and performance impact

Phase 1 activates the existing integer-centimeter/chunk foundation for parcels, facility footprints, safety clearance, work zones, movement pavement, public/service access, and temporary closure. Typed movement graphs and reservations are added without per-entity Actor Tick.

The one-aircraft slice is not permission to implement an O(n²) routing or validation path. Dirty-chunk/component propagation, graph revisions, bounded path caches, and diagnostics must be present where Phase 1 topology changes.

## Content and provenance impact

Primary Data Asset schemas and a validated catalog are introduced in `AMSimGameplay`; immutable projections enter `AMSimSimulation`. A project-owned temperate map replaces the engine Entry map.

The stable aircraft identity is `Aircraft.LightPiston.Starter`. Internally authored 2D content is the default implementation path until a player-facing content lock passes. No candidate file from `C:\Users\dave\Documents\Joes_Game\dist\assets` may be copied merely to unblock development.

## UI, input, and accessibility impact

The Phase 0 proof widget becomes a query-driven CommonUI flow. Semantic Enhanced Input actions cover camera, selection, construction, panels, and time controls. UI scale, redundant status encoding, captions, short corrective text, and explicit loading/empty/failure states are acceptance requirements rather than polish.

VA-01, VA-02, VA-04, and VA-05 guide hierarchy and world/UI balance. They do not control dimensions, values, geometry, or behavior.

## Offline and dependency impact

Runtime remains fully offline. Phase 1 may evaluate Unreal's local TextToSpeech provider only behind the `SpeechProvider` abstraction. Captions-first and local-cue fallback are mandatory. Provider failure cannot affect simulation timing or block the journey.

Shipping continues to exclude editor/test/MCP/Python/RemoteControl modules, listeners, debug commands, and unexpected endpoints. The elevated network-denied package journey remains a phase gate.

## Observability impact

Diagnostics expand to cover:

- per-domain entity/state counts;
- simulation step/backlog and command rejection reasons;
- dirty chunks, topology revisions, path/cache and reservation waits;
- construction stage and task blockers;
- offer/flight/ATC/turnaround state;
- transaction reconciliation and rating reasons;
- phrase queue/provider fallback;
- save/migration state and deterministic checksum.

Player-facing diagnostics translate these into stable cause/remedy messages and omit local paths or implementation names.

## Required reviews and evidence

Before Phase 1 closes:

- content/provenance and aviation-accuracy review;
- UI/accessibility and child-comprehension review;
- architecture/adversarial review of the integrated slice;
- representative reference-hardware selection;
- schema-1 migration and schema-2 corruption review;
- `S01` deterministic, rendered, save/load, performance, and new-tester evidence;
- `S15` network-denied Shipping evidence;
- visual-reference comparisons and package-boundary scans.

## Accepted implementation order

The implementation order is PI-03 work packages P1-00 through P1-08. Each package extends the same `S01` fixture and must preserve Phase 0 regression evidence.

No unresolved architecture conflict blocks P1-00 or P1-01. Player-facing content approval and representative hardware are exit locks, not reasons to hard-code content or defer testing.
