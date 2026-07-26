# Phase 1 Closeout Hardening Architecture Impact

**Status:** Passed
**Date:** 2026-07-26
**Scope:** performance evidence, flight catch-up, package acceptance harnesses

## Affected ownership

- `AMSimSimulation` retains authoritative flight progression and now applies
  catch-up transitions sequentially so intermediate reservations, services,
  phrase intents, events, and rewards cannot be skipped.
- `AMSimGameplay` exposes only the last world-subsystem simulation-work duration
  to the packaged evidence collector. The value is diagnostic and is not saved,
  checksummed, or used to make gameplay decisions.
- `AMSimUI` adds a Development-only local-speech failure switch for the documented
  human fallback review. Captions remain authoritative and the switch cannot
  change simulation state.
- `scripts/phase1` owns the named scalability profile, automation-report
  enforcement, physical hardware inventory, package hashing, and four-hour memory
  sampling.

## Interface and data impact

`UAMSimAirportSimulationSubsystem` adds
`GetLastSimulationWorkMilliseconds()`. Packaged smoke JSON adds simulation
median/p99, measurement duration, memory range, and applied scalability values.
No save field, snapshot schema, Primary Asset schema, stable content ID, or
command/event/query contract changes.

## Preserved invariants

- fixed-step time and authoritative state remain deterministic;
- catch-up produces the same ordered effects as ordinary stepwise progress;
- metrics and audio presentation cannot authorize state changes;
- reference instrumentation remains excluded from Shipping behavior;
- Shipping still contains no editor/test/MCP dependencies or required 3D asset.

## Migration and rollback

No migration is required. Rolling back the diagnostics removes evidence fields but
does not invalidate schema-2 saves. Rolling back sequential catch-up would restore
a verified no-skip defect and is not acceptable.

## Verification

- editor target builds;
- 25/25 AMSim tests pass, including forced full-flight catch-up;
- all five UI scales and the final Development/Shipping package pipeline pass;
- packaged smoke records the reference profile and 1x simulation percentiles;
- the reference-tier script completes a five-second non-certifying harness run.
