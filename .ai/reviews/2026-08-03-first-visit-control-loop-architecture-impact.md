# First-visit control loop — architecture impact

Date: 2026-08-03

## Scope

Repair the player-facing dead end between starter-airfield completion and the
first autonomous GA visit. This is a presentation and interaction change. It
does not add construction types, alter flight outcomes, or change save schema.

## Boundaries

- `AMSimSimulation` remains authoritative for construction, scheduling, time,
  flight state, rewards, and Phase 2 unlocks.
- `AMSimUI` derives a concise operations-hub state from immutable Phase 1
  query/state snapshots and issues only existing schedule and speed commands.
- The Build tool reopens a project/status surface after funding; it does not
  imply that a committed starter layout can be edited.
- Schedule exposes the existing exact-slot picker and an explicit, reversible
  advance-to-arrival control. Acceleration returns to 1x when the aircraft
  reaches the visible inbound state.
- Overlay selection is transient presentation state on the world presenter.
  It does not enter saves or simulation checksums.

## Organization

Add focused operations-hub presentation/view/navigation files rather than
expanding the near-limit root presenter. The root screen owns only references
and orchestration. No runtime string asset load, editor dependency, online
dependency, or new cooked asset is introduced.

## Compatibility

Schema 1–9 saves remain unchanged. A loaded scheduled visit derives the same
hub state. The optional auto-return flag is deliberately session-local and is
cleared whenever the player selects another speed or the inbound boundary is
reached.

## Decision

Proceed. This closes an interaction gap while preserving the established
C++ simulation / presentation-only UI boundary.
