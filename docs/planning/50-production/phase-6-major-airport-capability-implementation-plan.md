# PI-09: Phase 6 Major-Airport Capability and Scale

**Status:** Technical implementation and automated verification complete;
owner acceptance open
**Owner:** Simulation, gameplay, presentation, and content
**Approved:** 2026-07-28
**Normative requirements:** GS-04 through GS-20, CT-01, CT-02, CT-05,
TS-01 through TS-09

## Outcome

Phase 6 completes the large-airport systems and allows General Aviation,
Flight School, Charter, Cargo, Passenger, and Mixed airports to reach
`Major` independently. It adds high-capacity facilities, parallel-runway
operations, one reviewed Boeing 787-9 passenger type, severe but recoverable
aircraft-loss incidents, and deterministic aggregation at the approved
maximum targets.

Phase 5's six owner-operated Advanced journeys do not block Phase 6
implementation, but they remain required before Phase 6 can close. The
four-hour physical-tier soak, formal unassisted testing, and one genuine
10-15 hour pacing journey remain Phase 7 gates.

## Major capability contract

All paths require 100 Airport Points, 14 qualifying operating days, 80 safety,
80 reliability, and 75 applicable tenant relationships. A qualifying day
contains a completed path operation and no unresolved critical readiness
failure at accounting close.

| Path | Additional Major evidence |
| --- | --- |
| General Aviation | Eight-aircraft hangar campus; active FBO, maintenance, and fuel; five GA roles; 36 movements; successful fly-in |
| Flight School | Active school and instruction center; four trainers; two qualified instructor teams; 36 training blocks; successful open day |
| Charter | Active charter tenant and executive facility; two business-jet stands; premium handling; 24 movements including four short-notice or VIP movements |
| Cargo | High-capacity cargo hub and cargo operator; 36 shipments; 12 dedicated-freighter movements; six night shipments; all classes and directions retained |
| Passenger | High-capacity terminal, security, baggage, and access; passenger operator and two concessions; 24 recurring flights; 500 journeys; one complete 787-9 turnaround; two public-access modes |
| Mixed | Three non-Mixed paths at Regional, two at Advanced, five shared-resource days, 60 combined operations, and one capacity conflict recovered without cancellation or unfinished mandatory work |

Capability state distinguishes the permanently earned band from the currently
operational band. Requirement loss starts grace/recovery and may suspend
Major operations, but never erases the earned unlock.

Every path must have enough universal and path-specific first-time AP sources
to reach 100 without repetitive grinding or another path's signature
facility.

## Player-visible systems

- Constructible parallel runway, extension, lighting, approach-aid, crossing,
  holding-point, large-stand, terminal-concourse, baggage, cargo, hangar,
  instruction, executive, depot, emergency, parking, bus, rail, and staff
  definitions use existing proposal, construction, economy, routing,
  dispatch, timetable, tenant, and staffing behavior.
- Runway numbers derive from magnetic heading, reciprocal ends differ by 18,
  and physical ordering assigns L/C/R. Ends support independent closures and
  arrival, departure, mixed, available, or closed use.
- The Boeing 787-9 entered after CT-01/CT-02 scale, silhouette, livery,
  accuracy, provenance, and import approval. Its retained runtime master is
  high-resolution, top-down, 2D, and fictional in operator/livery.
- The 787 requires compatible runway, taxi, stand, passenger, baggage,
  emergency, pushback, fueling, catering, cleaning, water, lavatory, and
  boarding capability. Cargo Major does not require the 787 or a passenger
  terminal.
- Serious aircraft loss requires a disclosed causal risk and acknowledged
  risky override or policy. Response, bounded closure, abstract outcome,
  investigation, repair, financial recovery, and safe reopening use ordinary
  systems and cannot permanently bankrupt the save.
- High-volume passengers, bags, aircraft, staff, and vehicles retain
  authoritative identity. Deterministic update bucketing and pooled
  presentation may reduce update/visual density without changing counts,
  ordering, queues, or outcomes.

## Technical contracts

- Add schema 7 and a composed `FPhase6Simulation` with typed commands/results,
  causal events, stable records, immutable revisioned queries, deterministic
  checksums, fixtures, restore validation, and Phase 6 access through
  `FSimulation` and `UAMSimAirportSimulationSubsystem`.
- Keep Phase 5 as progression authority. Factor progression into a focused
  evaluator; Phase 6 supplies immutable Major evidence signals and never
  mutates Phase 5 directly.
- Generalize shared runway, stand, aircraft, wake, service-port, and
  compatibility data so Phase 4-6 operations use the same checks rather than
  decorative Phase 6 counters.
- Schemas 1-6 migrate to an empty Phase 6 state and preserve all legacy
  domains. Existing capability bands migrate into earned and operational
  state; Major evidence is derived only from preserved authoritative facts.
- Add `/Game/Phase6` Primary Assets and explicit cook coverage. Required
  runtime assets use serialized UObject references or reviewed Asset Manager
  references, never runtime string loads.
- Keep new simulation, serialization, presentation, and tests in focused
  files. Existing near-limit Phase 5, root, regional, and world-presenter
  files must remain below the 2,000-line standard.

## Presentation and visual validation

Add dedicated Major Operations, Runway Configuration, Capacity/Flow,
Large-Aircraft Turnaround, and Serious Incident views. Extend the capability
map so all six Major paths are evidence-backed, non-exclusive, and not an
ending.

Create five Figma-equivalent Phase 6 validation states:

1. parallel-runway expansion and Major overview against VA-01, VA-02, VA-04;
2. high-capacity terminal, baggage, and ground access against VA-03;
3. Boeing 787-9 turnaround against VA-05;
4. serious incident, bounded closure, response, and recovery against VA-06;
5. six-path Major capability map against VA-07.

Capture at 1920x1080 and 100%, 125%, 150%, 175%, and 200%. Preserve strict
top-down 2D, the Riverbend component language, concise copy, color-independent
state, a visible mouse, at least 60% world area in normal operations, and
compact drawers at 175-200%.

There are two visual approval checkpoints: the 787 scale/livery sheet before
cross-repository intake, and the five final equivalent-state boards.

## Work sequence

1. Lock PI-09 IDs, balance, evidence, fixtures, interfaces, visual
   measurements, architecture impact, and adversarial controls.
2. Implement schema 7, Phase 6 contracts, earned/operational bands, AP
   coverage, migration, and six headless Major fixtures.
3. Implement shared runway/configuration, compatibility, high-capacity
   facilities, ordinary-system integration, and the 787 approval checkpoint.
4. Implement the six late-game path systems, shared-resource pressure, serious
   incident/recovery, and deterministic scale aggregation.
5. Implement dedicated UI and Paper2D presentation, reviewed content,
   responsive captures, Figma frames, and comparison corrections.
6. Run full automation, migration, determinism, performance, packages, cook
   inventory, offline smoke, and forbidden-dependency/runtime-load/3D scans.
7. Complete the six open Phase 5 Advanced journeys and six Phase 6 prepared
   Major journeys, then update context, traceability, verification, reviews,
   provenance, and coherent source control.

## Acceptance

- Six independent deterministic simulations reach Major with unrelated
  signature facilities absent and remain solvent.
- Major cannot unlock before 100 AP and 14 qualifying days; earned and
  operational capability, grace, suspension, and recovery are reproducible.
- Runway numbering, reciprocal/parallel suffixes, configuration, crossing,
  wake, weather, closures, and sequencing are deterministic.
- Large-aircraft compatibility and turnaround enforce every disclosed
  requirement; passenger, bag, cargo, service, economy, AP, rating, and tenant
  totals reconcile.
- Serious incidents prove warning provenance, seeded outcome, bounded
  closure, response, investigation, repair, recovery funding, save/load, and
  continued operation.
- Maximum fixtures exercise 10,000 logical agents, 2,000 visible agents, 150
  aircraft, and 500 vehicles within TS-06 budgets. A bounded 15-minute
  maximum-load run replaces the deferred four-hour soak.
- Schema 1-7 migration, corrupt/future rejection, deterministic continuation,
  focused/full automation, Development/Shipping packages, cooked inventory,
  offline smoke, and zero forbidden editor/MCP/test, runtime-string-load, 3D,
  or network dependencies pass.
- Six prepared Major saves require about 15-20 minutes each to complete the
  final evidence through player-facing controls.

## Content assumptions

The candidate source is
`C:\Users\dave\Documents\Joes_Game\dist\assets\Boeing_787-9-B24vQXZX.png`,
currently 719x776 RGBA with SHA-256
`6AA56CD2CDD7035E3EE3526CFF5A603751E03441DA11ECE6C7E6E550B9B04449`.
No copy occurs before approval. The source is intake/reference material; the
retained runtime master is project-authored at sufficient resolution with a
fictional livery and sixteen reviewed headings.

No other `Joes_Game` aircraft, military aircraft, real operator branding,
online dependency, 3D asset, or runtime string asset load enters Phase 6.
Unrelated user-owned `SourceAssets/Audio/Music` changes remain untouched and
outside Phase 6 commits.

## Technical acceptance evidence

| Gate | Result |
| --- | --- |
| Focused Phase 6 automation | 8 succeeded; zero warnings/failures |
| Full Phase 1-6 automation | 62 succeeded plus one retained warning-only legacy save test; zero failures/not-run |
| Six independent Major paths | Passed with unrelated signature facilities absent |
| Parallel-runway and wide-body integration | Passed through ordinary reservations, compatibility, dispatch, passenger, bag, economy, and save contracts |
| Serious incident continuity | Warning, decision, response, bounded closure, investigation, repair, funding, reopening, and persistence passed |
| Maximum-scale contract | 10,000 logical, 2,000 visible, 150 aircraft, and 500 vehicles passed with no backlog warning |
| Schema 7 and migration | Round-trip, schema-6 migration, corrupt/future rejection, and deterministic continuation passed |
| Development and Shipping BuildCookRun | Passed; 613 cooked packages in each configuration |
| Shipping IoStore inventory | All 60 Phase 6 assets and the production root Widget Blueprint present |
| Shipping dependency boundary | Zero forbidden editor/MCP/test/tool files, manifests, or receipt matches |
| Runtime asset loading and 3D content | Zero direct runtime string asset loads and zero required 3D assets |
| Offline packaged smoke | Responsive after eight seconds; zero TCP connections |
| Visual validation | Five required 1920x1080 equivalent-state boards plus one maximum-scale frame and mismatch/correction records exported and reviewed |
| Aircraft content | Owner-approved River & Sun master, 16 headings, manifest, provenance, and cooker-visible texture/sprite pairs |

Machine-readable reports and package inventories remain under
`AMSim/Saved/Phase6` and `AMSim/Saved/Phase6PackagesFinalClean`. They are
reproducible local build artifacts and are not source-controlled.

## Remaining owner acceptance

- Complete the six previously deferred Phase 5 Advanced prepared-save
  journeys.
- Complete the six Phase 6 Major prepared-save journeys through player-facing
  controls.
- Review and accept the five required equivalent-state boards and supplemental
  maximum-scale frame at the owner visual checkpoint.
- Run the bounded 15-minute maximum-load observation. The deterministic
  maximum-scale contract already passes, but it does not substitute for the
  time-based observation.

The four-hour physical-tier soak, formal unassisted testing, and genuine
10-15-hour pacing journey remain Phase 7 gates.
