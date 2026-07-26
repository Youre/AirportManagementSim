# Phase 1 Plan Adversarial Review

**Status:** Passed with exit locks
**Date:** 2026-07-26
**Scope:** PI-03 planning completeness, contradictions, regression risk, migration, security, and operability

## Review question

Could PI-03 appear to define a vertical slice while allowing disconnected systems, nondeterministic shortcuts, hard-coded content, unsafe saves, unreviewed aircraft art, authoritative presentation state, runtime network dependencies, or an unrepresentative performance claim?

## Findings resolved in planning

### First-flight timing contradiction

PX-01/GS-20 previously required the first aircraft within three game minutes after opening, while GS-04 restricts timetable placement to five-game-minute increments. An arbitrary opening time cannot satisfy both.

The owning specifications now define:

- the compatible starter offer appears immediately when the airport becomes ready;
- the game remains paused while the player accepts and schedules it;
- the first aircraft arrives within three game minutes after that schedule is confirmed.

This preserves the fast onboarding outcome without a one-off timetable rule or manipulated clock.

### Disconnected-system risk

A phase organized only by domain could land construction, scheduling, movement, and UI without a playable connection. PI-03 instead defines one stable `S01` fixture and requires every work package to extend its command journey and assertions.

### Content hard-coding and rights pressure

The slice needs a visible aircraft before the external candidate library is approved. The plan uses stable content ID `Aircraft.LightPiston.Starter` and internally authored, provenance-recorded 2D test art by default. Player-facing content review is an exit lock. No external file is copied as a shortcut.

### One-map selection ambiguity

Phase 1 provides one real temperate region rather than presenting nonfunctional warm-dry/cool-wet choices. The selection flow remains implemented and data-driven; later maps can be added through CT-04 without simulation changes.

### Generic foundation becoming a monolith

The Phase 0 generic command/event surface is not expanded into one large enum/switch implementation. PI-03 requires typed domain payloads, focused services/files, strong IDs, explicit validation, and domain query builders composed by `FSimulation`.

### Save compatibility deferral

Phase 1 cannot add authoritative records while leaving the snapshot at schema 1. PI-03 makes schema 2 and a retained pure schema-1 migration fixture part of P1-01, before UI/content integration.

### UI and animation authorizing outcomes

The plan explicitly keeps project completion, aircraft transitions, services, rewards, and progression in simulation rules. Actors, Widgets, audio, and animation consume events/queries and cannot authorize state changes.

### TTS/network leakage

Audible radio cannot justify a cloud service or mandatory experimental plugin. Phrase intents and captions are deterministic; one local provider may render audio, and captions plus a local cue are the failure path. Shipping/network-denied scans remain mandatory.

### High-end performance false confidence

The RTX 5090 Phase 0 host remains useful development evidence but cannot close TS-08 for Phase 1. Representative hardware/scalability selection and measurements are an explicit exit lock.

### One-aircraft algorithm shortcuts

The first slice displays one aircraft, but routing, reservations, dirty-topology propagation, and state ownership must follow the scalable TS-02/TS-03 contracts. The plan rejects per-entity authoritative Tick and unbounded or O(n²) paths hidden by the small fixture.

## Exit locks

The following do not block P1-00/P1-01 but block Phase 1 completion:

- player-facing aircraft/operator/art provenance and aviation-accuracy review;
- a representative reference hardware/scalability tier and TS-08 measurements;
- an audible local normal-path radio proof plus captions/local-cue failure proof;
- project-owned map and complete packaged catalog validation;
- new-tester completion/comprehension evidence;
- schema migration, visual comparison, Shipping boundary, and network-denied evidence.

## Scope pressure checks

The plan deliberately excludes multiple-aircraft congestion, recurring schedules, passenger/cargo/tenant systems, changing weather, incidents, advanced staffing, towing, pushback, controller support, and external services. Interfaces may anticipate later domains, but Phase 1 cannot implement them at the cost of the complete starter journey.

## Residual risks and controls

- Unreal binary assets are not meaningfully reviewable as text. Editor commandlets must emit deterministic inventory/validation reports and source-control remains the final changed-path authority.
- Exact player-facing light-piston identity and visual treatment are not yet locked. Stable content IDs and internal-test art prevent this from contaminating save contracts.
- Exact reference hardware is not yet selected. Performance results before selection are developmental, not release claims.
- The first schema migration will establish the fixture discipline used later. Schema-1 input must remain immutable in the repository.

No unresolved contradiction blocks implementation planning. PI-03 may proceed in work-package order.
