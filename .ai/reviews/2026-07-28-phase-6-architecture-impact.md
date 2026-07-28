# Phase 6 Architecture-Impact Review

**Status:** Passed; implementation controls verified
**Date:** 2026-07-28
**Scope:** PI-09 Major-airport capability and scale

## Decision

Phase 6 adds one deterministic integration domain beside Phases 1-5. It owns
parallel-runway configuration, high-capacity facility links, large-aircraft
operations, Major capability signals, scale diagnostics, serious incidents,
repairs, and recovery. It does not duplicate Phase 1 economy/AP, Phase 2
dispatch, Phase 3 passenger/bag identity, Phase 4 operational flights, or
Phase 5 progression authority.

## Ownership

- `AMSimSimulation` owns Phase 6 commands, state, validation, events,
  deterministic fixtures, schema-7 persistence, and scale scheduling.
- A focused progression evaluator remains the only writer of Phase 5
  capability records. Phase 6 contributes immutable Major evidence.
- Existing domains retain authoritative economy, tasks, flights, passengers,
  bags, cargo, tenants, ratings, routes, and weather. Phase 6 links stable IDs
  and consumes bounded summaries rather than copying truth.
- `AMSimGameplay` owns Phase 6 Primary Assets, catalog validation, orchestration,
  prepared fixtures, and packaged smoke.
- `AMSimUI` owns query-only Major, runway, capacity, large-aircraft, incident,
  and pooled top-down 2D presentation.
- `AMSimEditor`, Figma, and scripts own reviewed intake, asset generation,
  validation, cooking, inventory, and comparison evidence.

## Persistence and compatibility

Schema 7 stores the complete Phase 6 domain and earned/operational capability
state. Schemas 1-6 migrate without changing legacy domain truth. Restore must
reject invalid runway labels/configurations, broken entity links,
incompatible aircraft assignments, capacity/conservation failures, impossible
capability state, incidents without warning provenance, invalid repair or
recovery state, and unsupported future schemas.

The legacy Phase 2 selected-specialization field remains serialized and
non-authoritative.

## Scale and organization

Logical identity is preserved at maximum load. Stable-ID update buckets,
query paging, and pooled proxies may reduce update or presentation frequency
only. No authoritative per-entity Actor/Widget Tick is introduced.

Phase 5 simulation, root/regional views, world presenter, and central
serializer are near the 2,000-line standard. Phase 6 must use focused
progression, serialization, presentation, and test files and split an
existing seam only where the new integration requires it.

## Content and package impact

`/Game/Phase6` is an explicit Primary Asset and cooker root. The Boeing 787-9
requires a reviewed manifest before source intake, a project-authored
high-resolution fictional livery, sixteen headings, and serialized
cooker-visible references. No additional external aircraft, real airline
branding, military core content, required 3D asset, runtime string load,
network service, editor module, or MCP dependency may enter Shipping.

## Mandatory controls

- Preserve six independent, non-exclusive Major paths.
- Keep Cargo, GA, Flight School, Charter, and Mixed Major viable without a
  passenger terminal or wide-body.
- Keep earned capability permanent while operational capability can enter
  grace, suspension, and recovery.
- Use ordinary construction, routing, flight, dispatch, passenger, bag, cargo,
  tenant, rating, weather, incident, and economy systems.
- Require warning provenance and acknowledged risk before aircraft loss.
- Keep incident outcomes abstract, age-appropriate, bounded, and recoverable.
- Preserve deterministic ordering, conservation, save continuation, and
  reference validation at maximum scale.
- Fail content/cooking validation on missing required Phase 6 assets.
- Validate every new surface against the applicable concept requirements at
  all supported UI scales.

With these controls, PI-09 fits the approved simulation, save, 2D, offline,
child-safety, content, and performance architecture.

## Exit verification

- Schema 7 and schemas 1-6 migration pass without duplicating prior domain
  authority.
- The focused progression evaluator is the only capability-band writer;
  Phase 6 contributes evidence and keeps earned/operational state separate.
- The 787 turnaround creates 11 ordinary dispatch tasks and reconciles the
  existing passenger, bag, flight, economy, and persistence systems.
- New Phase 6 simulation, serialization, UI, world, content, and test work is
  split into focused files; all source files remain below 2,000 lines.
- `/Game/Phase6` contains 28 Primary Asset definitions, 16 texture headings,
  and 16 PaperSprites. Direct Shipping inventory contains all 60 assets.
- Clean Development and Shipping packages pass with zero forbidden
  editor/MCP/test dependencies, zero direct runtime string asset loads, zero
  required 3D assets, and zero Shipping TCP connections.

The remaining owner journeys and timed observation are acceptance evidence,
not unresolved architecture risks.
