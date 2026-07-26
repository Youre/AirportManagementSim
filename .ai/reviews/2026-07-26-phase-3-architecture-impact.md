# Phase 3 Architecture-Impact Review

**Status:** Passed with mandatory implementation controls
**Date:** 2026-07-26
**Scope:** PI-05 terminal and landside vertical slice

## Decision

Phase 3 extends the existing composition with a focused passenger-terminal domain. It does not add a module, replace the 250 ms fixed step, duplicate Phase 1 economy authority, or duplicate Phase 2 airframe/flight authority.

## Ownership

- `AMSimSimulation` owns terminal topology, passengers, bags, domestic security, landside counts, passenger teams/tenant, commands, events, queries, checksum, and schema-4 state.
- `AMSimGameplay` owns Phase 3 content resolution, lifecycle adapters, save orchestration, fixtures, and package smoke.
- `AMSimUI` owns the query-only terminal presenter and Paper2D proxies.
- `AMSimEditor` owns Phase 3 content, cooking, provenance, and dependency audits.
- `AMSimTests` owns development-only acceptance and scale fixtures.

No Actor, Widget, PaperSprite component, Blueprint, screenshot, or editor/MCP action authorizes passenger, bag, security, route, transport, or flight outcomes.

## Persistence impact

Schema 4 adds the complete Phase 3 domain. Schemas 1-3 migrate to an empty Phase 3 state. Bounded restore validation covers stable IDs, ownership, counts, references, lifecycle combinations, controlled transitions, accessible paths, and exact reconciliation.

Selected passenger, open panel, route cache, proxy assignment, interpolation, world-label placement, and overlay animation remain presentation state and are not serialized.

## Scale impact

Passenger and bag decisions use deterministic buckets; topology and queue transitions remain bounded. The domain supports 10,000 logical passengers with no authoritative per-entity Actor Tick. Presentation uses a capped 2,000-proxy pool and aggregation outside detail zoom.

The normal Phase 3 fixture remains small enough to visually inspect every party and bag. The large fixture is separate and machine-verifiable so visual density cannot hide count loss.

## UI impact

The existing root implementation is already near the repository file-length limit. Phase 3 therefore uses a dedicated terminal widget/presenter with its own view-state mapping and command adapter. The accepted top bar, theme tokens, rounded surfaces, mouse policy, save/load, time controls, captions, and compact-scale rules remain shared.

VA-03 defines a state-specific terminal composition rather than a new global theme. The terminal world remains dominant and the persistent overlay uses the accepted deep-navy/cyan/amber/coral language.

## Content and package impact

Phase 3 uses only project-authored fictional 2D content. No file is copied from the external aircraft directory. The passenger aircraft may use the existing reviewed fictional presentation silhouette until a later CT-02 import is approved.

Phase 3 definitions and presentation descriptors use stable Primary Asset identities, serialized references, and an explicit `/Game/Phase3` cooker root. Shipping inventory is compared name-for-name with the source catalog. Runtime string loading is prohibited.

## Security and privacy impact

All passenger names and profiles are fictional, local to the save, and never exported. No protected trait affects screening, service, or transport eligibility. Security outcomes remain abstract and age-appropriate. No backend, account, telemetry, microphone, or network service is added.

## Mandatory controls

- Keep Phase 3 source files focused and below 2,000 lines.
- Keep Phase 1 credits and Phase 2 airframes authoritative through typed integration boundaries.
- Validate controlled topology both when opening a terminal and when restoring a save.
- Reconcile passenger, bag, aircraft, terminal, and regional totals every step.
- Separate actual logical population from capped presentation proxies.
- Fail content audit/cooking on missing Phase 3 assets.
- Run equivalent-state VA-03 comparison before visual acceptance.
- Prove every exit-critical command through the normal UI.

With these controls, the phase fits the approved module, save, UI, 2D, offline, and performance architecture.
