# TS-02: Deterministic Simulation and Entity Model

**Status:** Approved baseline  
**Owner:** Simulation core  
**Last updated:** 2026-07-25

## Outcome

Airport outcomes are reproducible, saveable, testable without rendering, and scalable to 10,000 logical agents. Authoritative entities are lightweight C++ records; Unreal objects are views or lifecycle services.

## Clock and determinism

The simulation uses a fixed logical step of 250 game milliseconds. Speed controls decide how many fixed steps are processed per rendered frame; pause processes no time steps but may accept planning commands. A frame has a capped simulation-work budget and carries remaining steps forward, displaying a performance warning rather than silently skipping authoritative time.

Determinism requires:

- one save-owned master seed and named random streams per domain;
- stable iteration order by stable ID, never container hash order;
- integer fixed-point values for money, capacities, clock, probabilities, and quantities where equality matters;
- no wall clock, frame delta, platform locale, pointer, Actor order, or asynchronous completion order in outcomes;
- versioned rules and definition references stored with snapshots.

Bit-identical replay is required for supported Windows builds with the same simulation/content versions. Migration may intentionally change representation but must preserve documented outcomes.

## Entities and components

An entity has `FSimEntityId`, type tag, lifecycle state, creation sequence, and domain-owned components. Components are plain serializable records grouped by concern: transform/path progress, schedule, inventory, need, task, reservation, ownership, finance, risk, and presentation descriptor.

High-volume collections use dense storage plus stable-ID lookup and deferred mutation. Creation and destruction occur only at command/step barriers. Removed IDs are tombstoned for the current event window and never reused within a save.

## Commands, events, and queries

A command includes command ID, type, requested game time, issuer, payload, and optional precondition revision. Validation returns accepted, rejected with reason codes, or accepted-with-warning requiring explicit confirmation.

Events are immutable facts with sequence, game time, type, involved IDs, cause command/event, and payload. Event order is canonical. Events drive feedback and short audit history; they are not an unbounded event-sourced save.

At the end of a step, each domain publishes immutable, revisioned query views. UI can request summaries and paged detail but cannot retain pointers into mutable state.

## Update scheduling and scale

Critical movement/reservations update every step. Queues, tasks, and close passenger decisions update in scheduled buckets. Distant passenger needs, ratings, offers, and forecasts update at coarser documented intervals. Bucketing may delay a decision within its tolerance but cannot change counts or ordering based on frame rate.

Presentation relevance selects at most the supported visible target; every offscreen/aggregated entity remains logically simulated. There is no authoritative Actor Tick per passenger, bag, staff member, or vehicle.

## Replay and diagnostics

Test and debug builds can record initial snapshot, content hashes, command stream, random-stream counters, periodic state checksums, and event digests. A divergence report identifies first mismatched step and domain. Player release saves need not retain the full command history.

## Acceptance

A headless run replayed twice must produce matching checksums and events. Save/load at arbitrary boundaries must match uninterrupted execution. Tests cover stable ordering, pause commands, speed independence, random streams, creation/destruction, coarse buckets, 10,000 agents, and rejection of nondeterministic APIs in the simulation module.

