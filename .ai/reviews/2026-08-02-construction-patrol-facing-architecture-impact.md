# Construction Patrol and Movement Facing Architecture Impact

Date: 2026-08-02
Status: Approved for implementation

## User-visible outcome

Construction workers visibly walk short, repeated patrols along the runway,
taxiway, gate approach, and optional road while surface work or inspection is
active. Workers and the delivery truck face the direction in which they are
currently traveling. Pause freezes both position and facing; normal time-speed
controls accelerate them with the rest of the game.

## Authority boundary

- Phase 1 simulation remains authoritative for project stage, game time,
  delivery, construction progress, and completion.
- Patrol positions and facing are deterministic presentation values derived
  from immutable project geometry and authoritative game time.
- No actor tick, wall clock, collision, pathfinding result, or component
  transform feeds back into simulation or persistence.
- Save schema and construction balance remain unchanged.

## Presentation design

- Replace the surface-progress sweep with a triangular patrol cycle so each
  worker walks out and back without teleporting at the cycle boundary.
- Use staggered phases and bounded work zones to keep four workers distinct.
- Assign two workers to separate runway zones, one to an active taxi segment,
  and one to the optional road or the gate-side taxi segment.
- Continue the patrol during inspection so visible workers do not become
  statues before the project opens.
- Centralize the Paper2D forward-axis correction in the Phase 1 world-geometry
  helper. The current worker and truck art faces local negative Y before world
  yaw is applied.

## Verification and rollback

Focused tests cover triangular continuity, staggered patrols, game-time motion,
pause stability through unchanged revision/time, and the four cardinal facing
directions. Rendered construction captures verify worker placement and sprite
orientation. Rollback restores the previous surface-progress positioning;
there is no data migration or asset rollback.
