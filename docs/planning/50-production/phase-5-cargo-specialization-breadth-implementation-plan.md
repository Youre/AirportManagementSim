# PI-08: Phase 5 Cargo and Specialization Breadth

**Status:** Implementation and technical verification complete; owner accelerated acceptance open
**Owner:** Simulation, gameplay, presentation, and content  
**Approved:** 2026-07-27  
**Completed:** 2026-07-27  
**Normative requirements:** GS-04, GS-07, GS-12, GS-14 through GS-16,
GS-19, GS-20, CT-01, CT-05, TS-01 through TS-09

## Outcome

Phase 5 makes cargo a complete operating path and advances General Aviation,
Flight School, Charter, Cargo, Passenger, and Mixed airports independently to
the `Advanced` capability band. It retains the Phase 1 economy/AP authority,
ordinary Phase 2 service dispatch, Phase 3 passenger/bag reconciliation, and
Phase 4 operational flights.

Major capability, wide-body operations, the four-hour physical-tier soak, and
formal unassisted testing remain Phase 6 or Phase 7 work.

## Delivered contracts

- Schema 6 composes `FPhase5State` into `FSimulation`, with stable IDs, typed
  commands/results/events, immutable revisioned queries, deterministic
  checksums, strict restore validation, and schema 1-5 migration.
- Cargo supports general, express/mail, temperature-sensitive, and oversized
  freight; outbound, inbound, transfer, and compatible belly flows; pieces,
  mass, volume, security, deadline, location, storage, task, flight, exception,
  and reward ownership. Deterministic recurring offers keep the board viable
  beyond the five authored fixture journeys and allow Cargo to reach its
  12-shipment Advanced threshold.
- A road/airside-connected warehouse exposes compatible general, express,
  cold, and oversized capacity. Quantity-based cargo work uses the ordinary
  dispatcher and its forklift, dolly/tug, truck, loader, support-unit, and
  cargo-team resources.
- Dedicated feeder and regional freighters use existing flight, stand,
  runway, timetable, turnaround, staff, vehicle, weather, and economy
  contracts. Belly freight links to compatible Phase 4 passenger-flight IDs.
- FBO, maintenance, fuel, cargo-operator, food, and retail offers expose
  footprint, opening cost, daily rent, revenue share, required capability,
  service expectations, operating pattern, satisfaction drivers, term, and
  visible grace/recovery.
- Per-path evidence replaces exclusive specialization authority after Phase 5
  initialization. The legacy Phase 2 selection remains serialized for schema
  compatibility only.
- All eight GS-19 event families share offer, preview, accept/decline,
  preparation, active, partial-success, cleanup, completion, cooldown, and
  mid-lifecycle save contracts. Linked ordinary-system entities determine
  event outcomes.
- Exactly three contextual objectives, path-neutral achievements, and
  reproducible rating contributions are authoritative simulation records.

## Presentation

Dedicated Cargo, Tenant/Provider, Special Event, and Capability views use the
Riverbend rounded component language and concise, color-independent state.
The central Paper2D world retains at least 60% of normal-operation width and
adds pooled warehouse zones, docks, airside ports, routes, cargo stacks,
vehicles, temporary event areas, exhibits, visitors, closures, and both
freighters. Presentation consumes immutable snapshots and never mutates
simulation positions.

The supplemental Figma validation source is
[Riverbend Phase 5 Validation](https://www.figma.com/design/I95YgVTwxL8irg3vsUrrBv).
Repository exports and discrepancy records are under
[`phase5-visual-validation`](phase5-visual-validation/README.md).

## Content

`/Game/Phase5` contains 44 Primary Asset definitions and four project-authored
aircraft presentation assets. The Riverlark F28 and Hearthwing F62 are
fictional top-down 2D freighters derived from editable SVG masters. Phase 5
imports no `Joes_Game` file, external aircraft art, military aircraft, or 3D
asset. All runtime references are cooker-visible UObject references.

See [Phase 5 content provenance](../30-content-and-assets/phase-5-content-provenance.md).

## Acceptance evidence

| Gate | Result |
| --- | --- |
| Focused Phase 5 automation | 5 succeeded, zero warnings/failures |
| Full Phase 1-5 automation | 54 succeeded, one retained warning-only legacy save test, zero failures/not-run |
| Six independent Advanced paths | Passed with unrelated signature facilities absent |
| Cargo classes, directions, dispatch, conservation, recovery | Passed |
| Tenants, grace/recovery, eight events, partial success | Passed |
| Schema 1-6, corrupt/future/reference rejection | Passed |
| Development and Shipping BuildCookRun | Passed; 553 cooked packages |
| Shipping IoStore inventory | 48 Phase 5 assets, represented by 50 entries including two texture bulk-data entries |
| Shipping dependency boundary | Zero forbidden executable/module matches |
| Runtime string asset loading | Zero outside editor-only asset tooling |
| Required 3D content | Zero |
| Offline packaged smoke | Responsive after eight seconds; zero TCP connections |
| Visual validation | Four Figma-equivalent states exported and reviewed |

Local machine-readable reports remain under `AMSim/Saved/Phase5` and
`AMSim/Saved/Phase5PackagesContractFinal`; they are reproducible build
artifacts and are not source-controlled.

## Remaining acceptance

- Phase 4.5 is accepted as the technical and visual-content baseline, while
  further aesthetic polish remains continuing visual debt rather than a Phase
  5 gameplay blocker.
- Six deterministic Advanced-path fixtures pass, but they do not substitute
  for the plan's six owner-operated 10-15 minute prepared-save journeys. That
  is the sole Phase 5 acceptance gate still open.
- Longer play sessions, the four-hour soak, and the formal unassisted protocol
  remain Phase 7 gates.
- Major capability and wide-body content are now technically implemented by
  PI-09; their owner acceptance gates remain open.
