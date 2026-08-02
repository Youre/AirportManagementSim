# PI-03: Phase 1 Grass-Airfield Implementation Plan

**Status:** Systems implementation verified; visual and interaction polish active
**Owner:** Production and integration
**Last updated:** 2026-08-02
**Roadmap phase:** Phase 1

Implementation evidence is recorded in the [Phase 1 verification record](../../../.ai/reviews/2026-07-26-phase-1-verification.md). All P1-00 through P1-08 systems exits, the reviewed player-facing content lock, the code/package gates, and elevated network-denied S15 pass. Active follow-up is visual fidelity and interaction polish, including concept-art comparison and persistent mouse behavior. The four-hour physical-tier measurement and [formal unassisted new-tester/audio/comprehension protocol](phase-1-new-tester-protocol.md) are retained but deferred to Phase 7 release hardening.

## Purpose

Phase 1 turns the Phase 0 empty-airport foundation into the first complete player journey:

`select temperate region -> name/save airport -> plan and build a grass airfield -> open it -> accept and schedule one compatible GA visit -> observe arrival, taxi, inspection/fueling, and departure -> receive an explained reward -> save/load`

The player-facing target is that a new tester can finish that journey without developer intervention and explain why the aircraft was compatible, where it went, what it needed, and what the airport earned. Short iterative playtests improve the slice now; the formal unassisted evidence record is deferred to Phase 7.

## Authoritative scope

Phase 1 integrates the smallest coherent portions of:

- PX-01 through PX-04;
- GS-01 through GS-07;
- GS-11, GS-15, GS-16, GS-17, and GS-20;
- CT-01 through CT-04;
- TS-01 through TS-09;
- scenarios `S01 Starter grass airfield` and `S15 Network-denied clean package`.

Later-phase behavior stays represented by stable interfaces where necessary, but is not implemented merely because an owning specification describes the final release.

## Player-visible definition

### New airport

The player can:

- choose the one Phase 1 curated `Map.TemperateStarter` region;
- name the airport and choose an independent local save slot;
- enter a paused, north-up, project-owned 2D map;
- see the owned starter parcel, external road portal, construction access, starter credits, clear daylight weather, and three contextual guidance cards.

The map-selection surface presents only content that actually exists. Warm-dry and cool-wet maps remain later content rather than disabled promises.

### Starter construction

The Phase 1 catalog contains:

- one map-authored basic operations terminal shell with two preset
  gate/stand connection ports; it provides construction context but no Phase 3
  passenger, baggage, or security behavior;
- grass runway with heading-derived numbering and required markings/windsock;
- one or more player-drawn grass-compatible taxi segments forming a connected
  runway-to-gate network;
- optional player-drawn service road segments that can shorten construction
  worker travel without blocking the airfield when absent.

The player may compose these items in a planning layer and commit them as one deterministic starter-airfield project. Preview shows land ownership, footprint, safety clearance, connection validity, total quoted cost, closures, and a specific remedy for every blocking result.

The project visibly advances through funded, delivery, building, inspection,
and player-opened states. Confirmation immediately dispatches the visible
truck and construction crew; accepted runway, taxiway, and optional road
geometry begins as graded earthwork and reveals its final surface
proportionally during building. Runway markings wait for inspection. Worker
arrival, material delivery, project stage, and timing are authoritative Phase
1 state; movement and surface reveal remain presentation.

At 1x, the starter project now allocates 30 game minutes to travel, 30 minutes
to site preparation, two game hours to visible surface work, and 30 minutes to
inspection. Because one game minute advances per real second at 1x, the full
project takes three and a half real minutes without a useful service road and
2 minutes 48 seconds with its 20 percent road benefit. Pause and 2x-8x speed
controls remain available. The finished compacted runway uses edge, center,
threshold, and correctly faced reciprocal number markings; taxiways use their
own amber center/edge treatment, distinct from unmarked service roads.

The complete valid starter plan must cost no more than 3,500 of the initial 5,000 Credits.

### First flight

Once a runway, complete runway-to-gate taxi route, the inherited starter
terminal/gate, and required starter teams are valid and open:

- a compatible one-time GA offer appears immediately while the simulation remains paused;
- the player accepts it, chooses an exact five-minute slot, and assigns the stand;
- after schedule confirmation, the first aircraft arrives within three game minutes of resumed simulation;
- a merely scheduled aircraft remains off-map; it becomes visible only when it enters the airport area and remains visible while moving or parked;
- the timetable shows the arrival, stand occupancy/buffers, and departure;
- automated ATC selects the open runway and emits captioned arrival, landing, taxi, hold, and takeoff phrase intents;
- one persistent light-piston airframe visibly approaches, lands, rolls out, follows the committed taxiway network to the connected gate, parks, receives inspection and fuel service, follows that network back to the runway, takes off, departs the map, and retains its identity/history;
- routine ATC, taxi, and service dispatch are autonomous. The player is not expected to steer the aircraft or press a hidden continuation action; the time controls are the optional pacing control;
- at 1x the visible inbound-to-outbound journey lasts about one real minute, including about forty real seconds parked for inspection and fueling. At 8x it remains a readable accelerated journey rather than a same-frame state cascade;
- operational speech calls play in first-in-first-out order without interrupting an active call, including when simulation speed produces calls faster than they can be spoken;
- the economy records itemized service/flight revenue and awards the first-loop capability evidence;
- rating feedback identifies at least safety/readiness and operational-reliability contributions;
- a suggested objective may update, but ignoring it never blocks play.

### Save and recovery

The player can save/load:

- while paused before construction;
- during each construction stage;
- after opening;
- while the flight is inbound, taxiing, parked/servicing, and outbound;
- after reward recognition.

Continuation must match uninterrupted deterministic state and events. A zero-credit fixture with one safe runway and stand receives a clearly labeled recovery path without restarting.

## Content policy

The stable Phase 1 content identity is `Aircraft.LightPiston.Starter`, paired with the fictional Riverbend Trainer, Riverbend Flying Club, and Riverbend call sign. The reviewed player-facing asset is a project-authored 2D directional vector silhouette with 16 heading steps; it uses no copied external aircraft file.

CT-01/CT-02 review `CT02.Phase1.RiverbendTrainer.2026-07-26` passes for the fictional identity and livery, 8.3 m length, 11.0 m wingspan, four-place capacity, 600 m grass-runway compatibility envelope, color-independent direction, deliberately fictional `RB-021` registration, and controller terminology. The machine-readable record is `AMSim/Config/Phase1/Phase1ContentReview.json`; its source and derived-asset hashes are enforced by the project audit and final acceptance aggregate. Stable content IDs and the save contract did not change.

No file is copied from `C:\Users\dave\Documents\Joes_Game\dist\assets` unless a populated manifest row records its exact deployed filename, checksum, creator/license, reuse decision, cleanup, scale, directions, destination ID, and reviewer. Using internally authored art is the default path when that approval is unavailable.

## Architecture plan

### Simulation records

`AMSimSimulation` gains plain, serializable records for:

- airport/map/calendar/weather state;
- parcels and facility instances;
- construction projects, deliveries, work, closures, and inspection;
- typed movement-network nodes/edges and stand/runway reservations;
- staff teams and minimal task assignments;
- credits, Airport Points, transactions, objectives, and rating contributions;
- contract offer, scheduled flight, persistent airframe, ATC/movement, and turnaround tasks;
- structured phrase intents;
- player policies, help-seen state, and Phase 1 diagnostics.

Phase 1 introduces save-stable strong IDs for the domains used by the slice: airport, facility, network node/edge, construction project, staff team, aircraft instance, contract, flight, transaction, objective, and phrase intent. Static definitions use stable content IDs.

The existing command/event/query foundation becomes typed domain contracts rather than a growing generic switch:

- commands own requested mutations and explicit confirmation of warnings;
- domain services validate and apply commands at fixed-step boundaries;
- immutable events record accepted facts and causal IDs;
- query builders publish airport, build, schedule, flight, inspect, finance, guidance, alert, and HUD views;
- presentation adapters never retain mutable simulation pointers.

Domain files remain focused and composed by `FSimulation`; Phase 1 does not turn `FSimulation` into a monolithic implementation file.

### Content and Unreal lifecycle

`AMSimGameplay` owns Unreal-facing Primary Data Asset schemas, Asset Manager/catalog loading, map-entry orchestration, save-slot selection, simulation stepping/backlog policy, and translation of validated definitions into immutable simulation inputs.

Required Phase 1 definition families are:

- map/region and balance profile;
- facility/network piece;
- construction recipe;
- staff role/team;
- aircraft/operator;
- offer/contract;
- service task;
- objective/help/text;
- weather profile;
- visual/audio presentation descriptor.

The project creates a project-owned Phase 1 map instead of continuing to use `/Engine/Maps/Entry`. Runtime code must fail clearly on missing or invalid required definitions; it cannot synthesize values from display names.

### Save schema

The first Phase 1 authoritative-state merge increments the snapshot to schema 2 and adds a retained schema-1-to-schema-2 fixture.

Migration creates an empty Phase 1 domain state around the Phase 0 seed, clock, IDs, and revision without inventing a completed airport. The original schema-1 file remains untouched until migrated validation succeeds.

Schema 2 stores all Phase 1 authoritative records, strong-ID counters, named random streams, content/rules versions, and the release-manifest hash. It excludes Actors, Widgets, routes/caches that can be rebuilt, audio clips, and presentation proxies.

### Presentation and input

`AMSimUI` expands the CommonUI shell with:

- new-airport/map/slot flow;
- top status bar, build tool rail, activity rail, context panel, alerts, and time controls;
- build catalog, proposal preview, diagnostics, commit confirmation, and project panel;
- offer and seven-day timetable surface sized for the one-flight slice;
- runway, stand, project, team, flight, and aircraft inspect views;
- caption queue and contextual help;
- query-backed view models and typed command adapters;
- pooled Paper 2D proxies for construction, the aircraft, and service activity.

Enhanced Input gains semantic selection, build, confirm/cancel, focus, pause, 1x/2x/4x/8x, and primary panel actions. Raw key reads remain prohibited.

Visual comparison uses VA-01, VA-02, VA-04, and VA-05. Written behavior, accessibility, and strict 2D presentation override incidental concept-art details.

### Radio and audio

Simulation emits deterministic phrase intents. Captions render immediately and are sufficient for every operational meaning.

Phase 1 evaluates one fully local speech provider behind CT-03's `SpeechProvider` contract. The normal proof should audibly render the essential first-flight calls, but initialization or synthesis failure must fall back to captions plus a local radio cue without changing timing or simulation state. Shipping performs no network request and stores no microphone or personal data.

## Work packages

### P1-00: Fixture and exit-lock preparation

- Freeze stable Phase 1 IDs, the starter journey seed, and `Map.TemperateStarter`.
- Author the starter-airfield cost/capability table from GS-20.
- Create internal-test provenance records for the light-piston aircraft, fictional operator, facilities, map, and UI/audio content.
- Define the player-facing content-review checklist.
- Select and record a representative reference hardware/scalability tier before the phase gate; the Phase 0 RTX 5090 host remains a development measurement host only.

Exit: a machine-readable fixture describes the intended starting state, command journey, assertions, content IDs, seed, and hardware evidence requirement.

Artifacts:

- `AMSim/Config/Phase1/S01-StarterGrassAirfield.json`;
- `AMSim/Config/Phase1/Phase1ContentManifest.json`;
- [Phase 1 reference hardware](phase-1-reference-hardware.md);
- `.ai/styles/phase-1-grass-airfield-ui.md`.

### P1-01: Domain contracts and schema 2

- Add strong IDs and typed command/event/query contracts.
- Add Phase 1 state aggregates, invariants, diagnostics, and definition projections.
- Extend snapshots/checksums and implement schema-1 migration.
- Add isolated unit tests before connecting UI.

Exit: headless creation, snapshot round trip, migration, replay, and rejection fixtures pass.

### P1-02: Temperate map and static catalog

- Add Primary Data Asset schemas and validated Phase 1 definitions.
- Create the project-owned map, owned parcel, road portal, buildable/terrain layers, camera bounds, climate, and clear-day profile.
- Add Asset Manager rules, localization keys, tags, provenance, compatibility, and commandlet validation.

Exit: a clean package resolves the complete Phase 1 catalog and opens the project map with zero content errors.

### P1-03: Planning, construction, and networks

- Implement proposal geometry, live diagnostics, quoted cost, planning layer, and bundle commit.
- Implement project stages, funds reservation, delivery, construction team work, inspection, opening, closure, cancellation/refund, and save continuation.
- Implement the aircraft-movement and essential-access graph portions needed by the starter loop.

Exit: a headless player command stream builds and opens a valid starter airfield for at most 3,500 Credits; invalid variants produce stable cause/remedy results.

### P1-04: Guidance, economy, and basic feedback

- Implement starting credits, transaction ledger, reward recognition, starter capability evidence, optional objectives/help state, recovery grant/offer, and basic rating contributions.
- Add query-backed HUD/finance/guidance/alert models.

Exit: reconciliation, independent-save, recoverability, objective feasibility, and explainable rating fixtures pass.

### P1-05: Offer, timetable, and compatibility

- Generate one deterministic compatible GA offer after readiness.
- Implement accept/decline/pin contracts, exact five-minute slot, stand assignment, buffers, and blocked/high-risk/advisory validation.
- Create the persistent airframe and flight instance only through accepted contracts/schedules.

Exit: offer timing, compatibility, exact-slot ownership, stand exclusivity, and save continuation pass.

### P1-06: ATC, movement, turnaround, and teams

- Implement runway numbering/open state, automated first-flight sequencing, kinematic path progress, rolling reservations, taxi/stand movement, and no-teleport invariants.
- Implement construction, ramp, and fueling team availability plus inspection/fueling task dependencies and departure readiness.
- Emit structured phrase intents and delay/blocker diagnostics.

Exit: the seeded aircraft completes arrival-to-departure deterministically, with required clearances, reservations, services, reward, and persistent history.

### P1-07: Player interface, presentation, and audio

- Replace the proof shell with the complete Phase 1 CommonUI flow.
- Add semantic input, build/schedule/inspect surfaces, readable alerts, captions, and local speech/fallback.
- Add pooled sprites and 2D overlays; capture equivalent VA-01/02/04/05 states.

Exit: UI automation and visual/accessibility review pass at 1920 x 1080 and 100%, 125%, 150%, 175%, and 200% UI scale.

### P1-08: Vertical-slice hardening

- Run `S01` headless and rendered from new save through post-load departure.
- Run save interruption/corruption/migration and deterministic continuation matrices.
- Run reference-hardware performance, memory, save-latency, and 8x backlog tests.
- Produce clean Development/Shipping packages and elevated network-denied `S15`.
- Scan Shipping for editor/test/MCP/network leakage and required 3D gameplay assets.
- Conduct the new-tester completion/comprehension review and record visual-reference differences.

Exit: every locally executable Phase 1 systems gate and traceability row has machine-readable evidence. Visual and interaction review remains iterative. The physical reference-tier long soak and formal unassisted human-session records are deferred release-hardening evidence rather than Phase 1 development locks.

## Verification matrix

| Requirement | Automated evidence | Human evidence |
| --- | --- | --- |
| New airport and independent save | slot/state integration and cross-slot isolation tests | tester understands map/name/save flow |
| Valid starter build within budget | deterministic `S01` build and cost assertions | live preview and remedies are understandable |
| Construction is visible and persistent | stage/delivery/team/save fixtures | player recognizes progress and affected area |
| Immediate starter offer and first arrival within three game minutes after scheduling | seeded readiness, paused-offer, schedule, and arrival-timing tests | player understands compatibility |
| Exact flight and stand ownership | timetable/stand exclusivity tests | schedule surface is readable |
| Safe landing/taxi/service/departure | ATC, reservation, task, readiness, no-overlap tests | calls/captions and blockers are understandable |
| Explained reward and feedback | ledger/rating/objective reconciliation tests | player can state what was earned and why |
| Recoverable zero-credit state | recovery fixture | assistance is clearly labeled, not punitive |
| Save/load continuity | boundary matrix and uninterrupted checksum comparison | restored state is recognizable |
| 2D/offline Shipping boundary | package scans and network-denied `S15` | proof contains no required 3D presentation |
| Performance | reference-tier frame/sim/save/memory JSON | interaction remains responsive |

## Phase 1 development gate

Phase 1 is ready to hand off into subsequent feature development only when:

1. an owner hands-on walkthrough completes the full journey without a blocking interaction defect, and observed usability issues enter the visual iteration loop;
2. the compatible starter offer appears immediately at readiness and the first aircraft arrives within three game minutes after schedule confirmation;
3. the valid starter plan costs no more than 3,500 Credits and leaves at least 30% contingency;
4. a zero-credit safe airfield remains recoverable;
5. identical command streams and save/load continuations match checksums/events;
6. no authoritative state is owned solely by Actors, Widgets, audio, or animation;
7. the player-facing content and all copied source files, if any, pass provenance/accuracy review;
8. short development-host performance, simulation, save, backlog, and memory checks pass; the physical-tier four-hour soak remains a Phase 7 release-hardening gate;
9. the clean Shipping package runs `S01`/`S15` with network denied and without editor/MCP/test dependencies or required 3D gameplay assets;
10. VA-01, VA-02, VA-04, and VA-05 equivalent-state comparison boards, prioritized mismatches, accessibility checks, bounded iterations, and known differences are recorded;
11. the Windows mouse remains visible and interactive when the player clicks into the world or CommonUI.

The latest verified package identity is tracked in
`scripts/phase1/Phase1AcceptanceManifest.json`. Short local gates are refreshed
after retained runtime changes. The physical-tier and unassisted-tester
collectors remain available, but their strict aggregate becomes a Phase 7
release-candidate gate rather than a blocker for continued development.

## Explicit non-goals

Phase 1 does not add:

- passenger-terminal interiors, baggage, security, landside transport, cargo,
  tenants, or incidents; the map-authored starter operations shell is only an
  aircraft gate and construction anchor;
- multiple-aircraft congestion, recurring schedules, flight school, charter, towing, pushback, deicing, or changing weather;
- advanced staff scheduling/morale, full specialization progression, or a complete release roster;
- controller support, multiplayer, accounts, telemetry, cloud services, or runtime MCP;
- 3D gameplay assets;
- unreviewed external aircraft art or real-airline branding.

These exclusions may not be used to bypass stable interfaces required for later phases.

## Risks and controls

| Risk | Control |
| --- | --- |
| Vertical slice becomes a collection of disconnected systems | Every work package ends in an executable extension of `S01` |
| Generic Phase 0 records become a monolith | Typed domain records/services and focused files composed by `FSimulation` |
| Content becomes hard-coded to unblock gameplay | Primary Data Assets, stable IDs, localization keys, and catalog validation land before UI wiring |
| External aircraft rights delay the phase | Default to internally authored, provenance-recorded test/player-facing art |
| UI owns gameplay state | Query-backed view models and command adapters; save excludes UI/proxies |
| Async or frame timing changes outcomes | Fixed-step barriers, canonical queues, named streams, and replay checks at every tranche |
| TTS leaks network/runtime dependencies | Local provider abstraction, captions-first fallback, Shipping/network scans |
| High-end workstation hides performance problems | Run short budgets every phase and complete the physical reference-tier soak during Phase 7 |
| Phase 2 complexity is pulled forward | Explicit non-goals and one-aircraft/one-offer fixture |

## Rollback

Each work package must leave the previous package's tests passing. Schema 1 fixtures remain immutable. Schema 2 is not considered established until migration and rollback evidence pass. Content is referenced by stable IDs so internal-test presentation can be replaced without rewriting saves or simulation rules.
