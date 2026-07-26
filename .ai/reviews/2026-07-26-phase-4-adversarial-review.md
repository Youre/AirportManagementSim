# Phase 4 Adversarial Review

**Status:** Passed
**Date:** 2026-07-26
**Scope:** PI-06 completeness, timetable truth, reconciliation, incident safety,
persistence, visual fidelity, package isolation, and recovery

## Review question

Could Phase 4 look like a regional timetable and incident dashboard while the
seven-day operations, connections, bags, border flow, gate change, weather,
serious-incident provenance, save continuity, or recovery behavior remain
decorative?

## Risks and required countermeasures

### The timetable could be a static calendar

Every card must map to a save-stable flight instance with exact five-minute
arrival/departure, contract, aircraft, gate, and actual offset. Gate occupancy,
buffers, remote buses, locked horizon, and approach limits are validated by the
simulation. The screenshot cannot count as schedule evidence.

### Disruptions could be labels without propagation

Early and late offsets must change actual operational state. A late inbound must
propagate to its connection decision and transfer cutoff. A gate change must
update passenger, baggage, service, and boarding compatibility together or be
rejected.

### Passengers or bags could disappear during transfer

Every connection and transfer bag retains stable passenger/flight ownership.
The completed fixture must reconcile connected, missed, rebooked, boarded, and
completed cohorts plus accepted, transferred, rebooked, loaded, and completed
bags. Tests deliberately break references and counts on restore.

### International processing could be a badge

The authored international flight must require a valid controlled border route,
open immigration/customs capacity, and completed processing counts. Missing or
impossible border state blocks completion and restore validation.

### Rental cars or rail could be summary text

Each mode needs an open tenant/facility, capacity, service frequency, route,
and reconciled passengers. Disconnected, closed, and over-capacity cases require
stable causes rather than teleportation.

### Weather could reroll on load or affect every aircraft equally

Forecast entries, confidence, season, wind, visibility, and runway surface are
seeded authoritative state. Approach eligibility uses aircraft-specific limits.
Save/load must preserve the forecast and identical restriction decisions.

### The serious incident could be arbitrary punishment

The incident cannot activate without stored warning, cause, remedy, relevant
condition, acknowledged override, deterministic seed, and mitigation record.
Closure remains localized, human outcome abstract, and recovery available.
Content and UI must not depict graphic harm.

### Recovery could be an automatic hidden reset

Hold, divert, protection, tow, investigation, repair/reopen, and recovery-plan
steps are typed commands with events. The incident report must expose causes,
response, effect, and prevention. Other safe operations remain active.

### Save/load could preserve a screenshot but corrupt the week

Boundary fixtures cover published timetable, high-risk gate change, late
connection, transfer route, international processing, active incident,
investigation, recovery, and tenant renewal. Resumed continuation must match
uninterrupted checksums and events.

### VA-04 and VA-06 could be cited without being followed

The gate requires equivalent-state captures, combined side-by-side boards,
image inspection, three prioritized mismatches, a bounded iteration, and
recapture. Region hierarchy, rounded component language, information density,
localized risk, adjacent evidence, caption, closure, and recovery actions are
judged visibly, not inferred from code.

### The package could pass while omitting Phase 4 content

The source-to-IoStore asset name set must match exactly. Shipping scans must
find zero editor/MCP/test dependency, socket, runtime string asset load, or
required 3D candidate. The Development package must exercise `S06` and `S10`
offline.

## Exit review

Every countermeasure passed. Five focused Phase 4 tests and the complete
45-pass-plus-one-retained-warning suite verify deterministic state, schema 5,
content, presentation mapping, and the no-unwarned-serious-incident invariant.
The integrated S06/S10 fixture reconciles 21 flights, 24 connecting passengers,
32 transfer bags, 96 border passengers, rental/rail populations, the incident
report/recovery chain, and one renewal.

VA-04 and VA-06 were inspected in combined boards, then recaptured after
aircraft identity, copy-density, and high-scale clipping corrections. Clean
Development and Shipping packages pass; all 34 Phase 4 assets are directly
present in IoStore with zero forbidden, socket, runtime string-load, or
required 3D match. The normative results are in
[the Phase 4 verification record](2026-07-26-phase-4-verification.md).
