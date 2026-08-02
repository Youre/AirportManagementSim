# First GA Visit Readability Architecture Impact

**Date:** 2026-08-02
**Outcome:** Approved for implementation

## User-visible contract

The scheduled starter aircraft remains off-map until it is inbound. It then
autonomously flies the approach, lands, rolls to the taxi connection, follows
the committed taxiway network to its assigned gate, parks for visible automatic
inspection and fueling, retraces the taxi route, takes off, and leaves the map.
No steering or mandatory player action is added.

## Boundaries

- Flight-state timing remains authoritative deterministic Phase 1 simulation
  balance. Named offsets live in `FPhase1Fixture`, not UI code.
- Aircraft coordinates and heading remain presentation-only. A dedicated Phase
  1 adapter derives them from immutable query/state snapshots and committed
  geometry; coordinates never feed back into the simulation.
- The adapter is revision-driven and owns no Actor Tick. It follows actual
  runway and taxiway segments, including connected multi-segment networks.
- Speech ordering remains presentation-only. Phrase intents still originate in
  deterministic simulation state; the radio subsystem serializes local speech
  so a new call cannot interrupt the current call.
- Existing captions remain immediate and sufficient without audio.

## Persistence, content, and packaging

No schema or authoritative record shape changes. Existing saves retain their
scheduled arrival and current flight state; restored state applies the new
balance thresholds deterministically. No new runtime asset, cook rule, plugin,
network dependency, or 3D content is introduced.

## Organization

Aircraft kinematics and taxi-path derivation are added in focused files rather
than expanding the near-limit world presenter. Radio queue behavior remains in
the small radio/speech provider modules. Tests cover the pure presentation
adapter and the queue independently.
