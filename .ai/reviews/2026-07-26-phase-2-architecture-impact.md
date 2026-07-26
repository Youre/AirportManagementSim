# Phase 2 Architecture-Impact Review

**Status:** Passed with implementation controls
**Date:** 2026-07-26
**Scope:** PI-04 living general-aviation implementation

## Decision

Phase 2 extends the existing architecture through composition rather than replacing the proven Phase 1 domain. A focused living-airport domain in `AMSimSimulation` owns only the new Phase 2 state. It integrates with Phase 1 credits, Airport Points, transactions, airport identity, and starter facilities through typed commands so there is never a second authoritative balance or competing airport record.

## Module and ownership impact

- `AMSimSimulation` owns Phase 2 records, validation, deterministic stepping, commands, events, queries, checksum, and schema-3 serialization.
- `AMSimGameplay` owns data-asset resolution, fixtures, save orchestration, Unreal lifecycle, and query-to-presentation translation.
- `AMSimUI` owns reusable view state, semantic command adapters, and presentation built from the accepted Phase 1.5 component language.
- `AMSimEditor` owns content, reference, cooking, and dependency audits.
- `AMSimTests` owns development-only acceptance and remains absent from Shipping.

No Actor, Widget, Blueprint, PaperSprite component, sound, screenshot, MCP tool, or content-authoring script may authorize a gameplay outcome.

## Persistence impact

The save schema increments from 2 to 3. Schema 3 contains the Phase 1 domain plus the Phase 2 extension, stable ID counters, and named random-stream state. Schema-1 and schema-2 migrations remain exercised. Phase 2 continuation adds project logistics, movement/towing, turnaround/deicing, weather transition, and incident response boundaries.

The deserializer must retain count limits, finite-value checks, unique ID checks, reference integrity, lifecycle validation, backup behavior, and future-schema rejection. Presentation state and rebuildable caches remain excluded.

## Runtime and scaling impact

The fixed 250 ms authoritative step and no-skip backlog policy remain. Multiple aircraft, vehicles, staff teams, tasks, reservations, and incidents are simulation records processed in bounded collections. Paper2D proxies are pooled and presentation-only. There is no per-entity authoritative Actor Tick.

The Phase 2 gate uses fixture-sized content rather than release-scale population. Profiling still records step, query, snapshot, save, frame, memory, and backlog measures so growth can be compared to Phase 1.

## Content and rendering impact

Phase 2 requires at least four aircraft roles but does not authorize importing the external aircraft directory. Additional aircraft are internally authored, fictional, editable 2D silhouettes with review records. Required runtime assets use serialized UObject/Primary Asset references visible to the cooker; runtime string loading is prohibited.

The Shipping closeout additionally roots `/Game/Phase2` through the project
packaging settings. This supplements Primary Asset discovery for the bounded
definition catalog. The pipeline enumerates the indexed IoStore and compares
all 33 cooked asset names with the source catalog, so the directory rule cannot
silently mask a missing or renamed required definition.

The accepted UI theme, rounded component language, transparent world viewport, compact-scale behavior, and visible mouse policy remain shared foundations. Phase 2 adds components and proxies, not a second visual system.

## Security and package impact

All features remain offline and independent-save. No backend, account, telemetry, microphone, external speech, or remote asset dependency is added. Editor automation and Unreal MCP remain editor-only and must produce zero Shipping matches.

## Implementation controls

- Keep Phase 2 source files domain-focused and below the repository length thresholds.
- Use a single Phase 1 integration boundary for economy/progression effects.
- Use stable IDs and immutable snapshots across every module boundary.
- Make scenario fixtures declarative and independently repeatable.
- Fail development audits and cooking for missing production presentation/content.
- Prove all new commands through player-facing UI before closing the phase.

With these controls, the implementation does not require a new module, a new runtime service, or a departure from the approved technical specifications.
