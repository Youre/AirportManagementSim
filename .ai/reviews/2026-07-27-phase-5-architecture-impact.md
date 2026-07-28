# Phase 5 Architecture-Impact Review

**Status:** Passed with mandatory implementation controls
**Date:** 2026-07-27
**Scope:** PI-08 cargo and specialization breadth

## Decision

Phase 5 adds one deterministic integration domain beside Phases 1-4. It owns
new cargo, provider-tenant, event, rating-contribution, objective,
achievement, and capability-evidence records. It does not replace the fixed
250 ms step, duplicate the Phase 1 economy ledger, duplicate existing
passenger/bag truth, or make presentation state authoritative.

Phase 2's serialized selected-specialization value remains a legacy field.
Phase 5 introduces non-exclusive evidence records for all six paths and becomes
the progression authority after initialization.

## Ownership and integration

- `AMSimSimulation` owns Phase 5 commands, validation, state, events, cargo
  conservation, capability evaluation, deterministic fixtures, and schema-6
  persistence.
- Existing flight, stand, service, staff, vehicle, weather, passenger, bag,
  and economy domains remain authoritative. Phase 5 links through stable IDs
  and bounded typed integration rather than copying their state.
- `AMSimGameplay` owns Phase 5 Primary Assets, catalog validation, save
  orchestration, fixture adapters, and packaged smoke.
- `AMSimUI` owns query-only cargo, provider, event, progression, and pooled 2D
  presentation. Figma and screenshots are validation artifacts only.
- `AMSimEditor` and project scripts own project-authored asset creation,
  cooking, inventory, provenance, and dependency scans.

## Persistence impact

Schema 6 stores the complete Phase 5 domain. Schemas 1-5 migrate to an empty
Phase 5 state, then derive initial evidence from preserved Phase 1-4 state only
when Phase 5 initializes. Restore validates stable IDs, cargo mass/volume and
location conservation, storage compatibility/capacity, ordinary-task
ownership, cargo-flight links, tenant grace, event lifecycle/cooldown,
objective/achievement identity, rating contribution totals, and capability
evidence.

Widget selection, open drawers, Figma node IDs, proxy assignments, camera
position, and screenshot stages remain presentation state.

## Scale and organization impact

Cargo uses aggregate shipment entities and quantity-based tasks. Event families
share one lifecycle and generate bounded ordinary-system demands. New
presenters and schema helpers use focused files; the existing root, regional
view, and serializer files must not cross the 2,000-line limit.

Pooled Paper2D proxies remain bounded independently of logical shipment,
visitor, or task counts. No authoritative per-entity Actor or Widget Tick is
introduced.

## Content and package impact

`/Game/Phase5` is an explicit cooker root and Primary Asset scan path. New
aircraft, cargo props, vehicles, icons, surfaces, tenants, contracts, and event
content are project-authored and fictional in appearance. No external
`Joes_Game` file, military aircraft, required 3D asset, runtime string load,
network service, or editor/MCP dependency may enter Shipping.

## Mandatory controls

- Keep all six specialization paths non-exclusive and independently testable.
- Keep cargo viable without passenger facilities; keep Passenger, GA, Flight
  School, and Charter viable without cargo facilities.
- Advance shipments only from completed ordinary operational work.
- Reconcile pieces, mass, volume, storage occupancy, aircraft load, and
  completed cargo at every transition and restore.
- Store causal rating contributions and reproduce every displayed score.
- Implement all event families through shared lifecycle and ordinary systems.
- Preserve declining-without-penalty, partial success, cleanup, cooldown, and
  recovery.
- Keep Major and wide-body capability future-locked.
- Fail content/cooking validation on a missing Phase 5 production asset.
- Validate new surfaces against the applicable concept requirements at all
  supported UI scales.

With these controls, Phase 5 fits the approved simulation, save, 2D, offline,
child-safety, content, and performance architecture.
