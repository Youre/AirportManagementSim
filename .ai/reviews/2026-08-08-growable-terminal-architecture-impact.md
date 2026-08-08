# Growable terminal starter vertical slice — architecture impact

Date: 2026-08-08

## Scope

Replace fixed starter and passenger-terminal presentation geometry with a
ground-floor spatial terminal domain, editor, construction workflow, and pooled
world presentation. The change advances snapshots from schema 9 to schema 10.

## Boundaries

- `AMSimSimulation` owns grid cells, wall/door topology, objects, inferred
  rooms, validation, closures, construction jobs, transactions, deterministic
  migration, checksum, and revisioned queries.
- Phase 1 remains authoritative for credits and ordinary construction-worker
  availability. Terminal commands request atomic ledger changes rather than
  maintaining a second balance.
- `AMSimUI` owns pointer gestures, cutaway camera state, transient selection,
  placement ghosts, expandable tools, pooled proxies, and presentation-only
  animation. It never feeds proxy positions back into simulation.
- Existing passenger, bag, security, staff, tenant, and flight identities stay
  in the Phase 3 domain. Their authored fixture becomes spatial without
  changing its operational outcomes.
- Runtime art remains Paper2D/top-down with hard or registered asset references.
  MCP, editor toolsets, and generation tools remain development-only.

## Organization

Do not extend the near-limit root, terminal-view, passenger-simulation,
snapshot-serializer, or world-presenter source files with broad new behavior.
Add focused terminal-layout, topology, construction, serialization, editor,
camera-transition, and proxy-presentation files behind narrow orchestration
calls. Keep every source file at or below 2,000 lines.

## Compatibility and rollback

- Schema 10 writes new terminal-layout records after existing Phase 3 data.
- Schemas 1-9 migrate with deterministic authored layouts and preserve all
  existing non-presentation identities and progress.
- Restore rejects duplicate objects, overlapping incompatible footprints,
  invalid wall/door edges, broken jobs/transactions, impossible closures, and
  unreproducible ledger deltas.
- A rollback may retain schema-10 reading while disabling editing and rendering
  the derived roof/cutaway from the saved layout; it must not discard spatial
  state or write schema 9 over schema 10.

## Decision

Proceed after the two-frame visual gate. The change is cross-cutting but follows
existing deterministic simulation, typed-command, immutable-query, Paper2D,
and cooker-visible asset boundaries.
