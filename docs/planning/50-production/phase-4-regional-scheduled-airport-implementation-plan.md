# PI-06: Phase 4 Regional Scheduled Airport Implementation Plan

**Status:** Complete
**Owner:** Production and integration
**Opened:** 2026-07-26
**Completed:** 2026-07-26
**Roadmap phase:** Phase 4

## Purpose

Phase 4 turns the domestic terminal into a regional scheduled airport:

`accept recurring operators -> publish a seven-day timetable -> operate contact and remote gates -> connect passengers and bags -> process an international service -> respond to weather and a warned serious incident -> recover, renew tenants, and finish the week`

The phase extends the accepted Phase 1.5 component language and the Phase 3
passenger truth model. It does not add cargo-specialization gameplay, major-airport
scale, wide-body aircraft, or Phase 5 breadth.

## Authoritative scope

Phase 4 integrates the Phase 4 portions of:

- PX-02 and PX-03;
- GS-03 through GS-06, GS-08 through GS-10, GS-12, GS-13, GS-15 through
  GS-18, and GS-20;
- CT-01, CT-02, CT-03, and CT-04;
- TS-01 through TS-06, TS-08, and TS-09;
- scenarios `S06 Passenger connection and baggage transfer`,
  `S10 Serious warned-risk incident and recovery`, `S14 Save migration matrix`,
  and `S15 Network-denied clean package`.

The gate requires a deterministic seven-day passenger fixture containing early
and late flights, a valid gate change, weather restrictions, passenger and bag
connections, a missed connection with recovery, international border processing,
and a traceable serious incident.

## Player-visible definition

### Contracts and timetable

Three fictional operators offer recurring services using a regional turboprop,
regional jet, and narrow-body aircraft definition. The player accepts the
contracts and publishes a repeating seven-day timetable in five-game-minute
increments. Each flight card owns an exact arrival, departure, gate, aircraft,
operator, route, and risk state.

The timetable presents:

- contact Gates A1 and A2 plus remote Stand R1 with bus boarding;
- a 30-game-minute locked operational horizon;
- localized `Blocked`, `High risk`, and `Advisory` feasibility evidence;
- runway, stand, services, terminal, baggage, border, and weather evidence
  adjacent to the selected flight;
- early and late actual times without replacing planned identity;
- a traceable override path for a high-risk gate change.

### Connections, international processing, and access

Stable connecting passengers retain inbound and outbound flight ownership.
Transfer bags retain passenger, inbound flight, outbound flight, route, cutoff,
screening, and reconciliation state. A weather-delayed inbound service causes a
small authored missed-connection cohort; the player sees the cause and rebooking
result. No passenger or bag disappears to satisfy the fixture.

The international service passes through fictional document control and customs
using a continuous controlled route. Inspection results remain abstract,
age-appropriate, local to the save, and independent of protected traits.

Rental-car and rail modes extend the Phase 3 landside network. Both require an
open tenant/facility, capacity, service frequency, and a pedestrian connection.
Regional entry, terminal, aircraft, transfer, and landside totals reconcile.

### Weather, approach limits, incident, and recovery

A seeded seven-day seasonal forecast exposes near-term confidence, wind,
visibility, runway surface, runway recommendation, and aircraft-specific approach
limits. The fixture includes one weather restriction that affects only flights
whose aircraft or approach capability crosses the disclosed limit.

The serious incident records:

- the risk warning and displayed remedy;
- the relevant weather and approach limit;
- the accepted timetable override;
- the deterministic incident seed;
- dispatched resources and mitigation commands;
- affected-only runway closure;
- abstract `No injuries` outcome;
- investigation findings, financial/rating effect, and prevention action.

The player can hold departures, divert arrivals, protect the area, dispatch a
tow team, review the cause, and apply a recovery plan. Other airport operations
continue whenever their facilities remain safe.

### Tenant renewals

Each recurring operator tracks controllable delay, completion, connection
handling, safety communication, and satisfaction. Renewal is offered after the
week and retains a recovery route when performance is weak. One accepted renewal
proves the loop without making poor performance an irreversible failure spiral.

## Visual contract

### VA-04 timetable surface

At 1920 x 1080 and 100% scale, the timetable is the dominant planning surface.
It uses:

- deep-navy rounded chrome and off-white type;
- a seven-day tab strip and time-aligned gate lanes;
- short operator/flight/aircraft cards with exact slot ownership;
- cyan selection, amber patterned risk, coral blocked state, and green
  icon-plus-text readiness;
- a compact contract rail and an adjacent selected-flight feasibility rail;
- only the information needed to accept, publish, inspect, and recover a flight.

At 125-150%, the rail composition remains. At 175-200%, contract and feasibility
rails become bounded drawers while the timetable stays legible; text is not
shrunk below the project minimum.

### VA-06 incident surface

The equivalent incident state keeps the operational world dominant and adds:

- a weather/current-conditions rail with a six-hour confidence-bearing forecast;
- readable 2D rain treatment that does not obscure paths or status;
- a world-connected emergency route and localized affected-area closure;
- a right incident lifecycle rail with explicit completed/current/waiting stages;
- a caption bar, cause/consequence summary, and safe recovery actions;
- continued-operation evidence outside the closure.

No human harm, 3D gameplay asset, real operator branding, or generated
incidental value is used.

## Architecture plan

### Simulation composition

`AMSimSimulation` gains `FRegionalScheduledSimulation`, composed beside the
Phase 1-3 domains. It owns only recurring contracts, seven-day flight instances,
connection/transfer records, border-processing summaries, regional transport,
forecast/approach state, tenant renewal state, and the Phase 4 incident audit.

Phase 1 remains the economy owner, Phase 2 remains the persistent-airframe owner,
and Phase 3 remains the detailed terminal/passenger/bag owner. Phase 4 references
those boundaries through stable IDs and read-only state, then posts bounded
economy changes through the existing external-ledger interface.

All player mutations use typed Phase 4 commands applied at fixed-step
boundaries. The domain publishes immutable revisioned queries and causal events.
Canonical stable-ID ordering, named random streams, integer time/counts, and no
authoritative Actor or Widget Tick remain mandatory.

### Persistence

The save contract increments to schema 5. Schema 5 stores Phase 4 stable IDs,
contracts, flights, timetable publication, actual offsets, gates, transfers,
border/access summaries, forecast, approach limits, renewals, incident audit,
events, and completion/recovery state.

Schemas 1-4 migrate to an empty Phase 4 domain. Restore rejects duplicate IDs,
broken flight/contract/connection/bag references, overlapping gates, out-of-range
five-minute slots, missing remote buses, impossible border state, untraceable
serious incidents, non-reconciled completed weeks, and unsupported future
versions. UI selection, open drawers, proxy assignment, and screenshot state are
not serialized.

### Presentation

`AMSimUI` adds a dedicated regional-operations presenter hosted above the
terminal presenter. It consumes only Phase 4 query/state, submits typed commands,
and reuses the accepted Riverbend theme, button, panel, status, evidence,
timetable, caption, and compact-drawer templates.

The existing Phase 3 Paper2D terminal remains the world baseline. Phase 4 adds
presentation-only weather, closure, emergency-route, gate, remote-bus, and
regional-aircraft cues. Required assets use serialized or constructor-time
references, `/Game/Phase4` is explicitly cooked, and runtime string asset loading
is prohibited.

## Work packages

### P4-00: Contract, visual measurement, and content lock

- Freeze the Phase 4 seed, IDs, operators, aircraft roles, gates, seven-day
  fixture, forecast, connection cohort, transfer-bag count, international flow,
  incident risk chain, and renewal outcome.
- Record VA-04 and VA-06 region proportions, hierarchy, state encodings,
  responsive behavior, and written-specification corrections.
- Complete architecture-impact and preliminary adversarial reviews.

**Exit:** every Phase 4 record, command, definition, visual surface, and gate
assertion has an owner.

### P4-01: State contracts and schema 5

- Add Phase 4 records, commands, events, queries, validation, checksum, and
  fixture.
- Compose the domain through `FSimulation`.
- Serialize schema 5 and retain schema 1-4 migration coverage.

**Exit:** deterministic replay, round-trip, migration, future/corrupt/reference
rejection, and boundary continuation tests pass.

### P4-02: Seven-day scheduled operations

- Add recurring contracts, exact five-minute slots, locked horizon, gate
  exclusivity, contact/remote boarding, early/late offsets, gate change, runway
  approach limits, and tenant performance/renewal.
- Run the authored seven-day fixture through normal typed commands.

**Exit:** every planned and actual flight remains identifiable, compatible, and
recoverable through the week.

### P4-03: Connections, border, access, weather, and incident

- Add passenger connections, transfer bags, missed-connection recovery,
  immigration/customs, rental cars, rail, seeded forecasts, and seasonal
  operating categories.
- Add serious-incident warning provenance, response lifecycle, affected-only
  closure, report, repair/recovery, and child-safe outcome.

**Exit:** all regional, terminal, flight, connection, bag, border, and landside
counts reconcile; the incident can be explained and recovered.

### P4-04: VA-04/VA-06 UI and 2D presentation

- Build the seven-day timetable and adjacent evidence surface from the accepted
  component language.
- Build the weather/incident surface with localized route/closure overlays,
  lifecycle, caption, cause, consequence, and recovery controls.
- Validate all commands through player-facing controls and preserve the visible
  mouse policy.

**Exit:** equivalent-state comparisons pass at 100%, the 100-200% matrix remains
usable, and no P0-P2 visual mismatch remains.

### P4-05: Package and closeout

- Run focused and complete automation, deterministic replay, schema matrix,
  `S06`, `S10`, package/offline scans, and content validation.
- Capture VA-04, VA-06, and complete-week states at every supported UI scale.
- Build clean Development and Shipping packages, exercise the packaged fixture,
  enumerate every Phase 4 asset, and scan dependencies, sockets, runtime loads,
  and 3D candidates.
- Update traceability, current state, verification, provenance, visual review,
  design QA, roadmap, and planning index.

**Exit:** every roadmap gate is evidenced and the coherent Phase 4 change is
committed and pushed.

## Completion gate

Phase 4 is complete only when:

- three recurring fictional operators publish a valid seven-day timetable;
- exact five-minute slot and gate ownership, remote buses, locked-horizon
  warnings, and override traceability pass;
- early/late operation, gate change, weather restriction, and runway approach
  differences are deterministic;
- passenger connections, missed-connection rebooking, and transfer bags
  reconcile exactly;
- the international service completes immigration and customs without bypass;
- rental-car and rail journeys reconcile with terminal populations;
- the serious incident has warning provenance, deterministic outcome,
  affected-only closure, complete response/reporting, and a recovery path;
- tenant performance produces and accepts a valid renewal;
- save/load continues identically at timetable, disruption, transfer, border,
  incident, and recovery boundaries;
- all Phase 4 commands are available through player-facing UI;
- VA-04 and VA-06 comparison boards and the 100-200% scale matrix pass;
- Shipping contains every required Phase 4 asset with zero forbidden
  editor/MCP/test dependency, socket, runtime string-load, or required 3D match;
- planning, traceability, current state, verification, provenance, and review
  records are current;
- the completed change is committed and pushed.

The formal unassisted tester protocol and four-hour physical-tier soak remain
Phase 7 gates.

## Closure evidence

Every completion item above passed on 2026-07-26. Normative evidence is:

- [Phase 4 verification record](../../../.ai/reviews/2026-07-26-phase-4-verification.md);
- [Phase 4 adversarial review](../../../.ai/reviews/2026-07-26-phase-4-adversarial-review.md);
- [Phase 4 architecture-impact review](../../../.ai/reviews/2026-07-26-phase-4-architecture-impact.md);
- [Phase 4 visual review](phase-4-visual-review.md);
- [Phase 4 content provenance](../30-content-and-assets/phase-4-content-provenance.md);
- project-root `design-qa.md`;
- machine-readable local results under `AMSim/Saved/Phase4`,
  `AMSim/Saved/Automation/Phase4Complete`, and
  `AMSim/Saved/Automation/Phase4Full`.

Phase 5 remains unopened.
