# Phase 2 Adversarial Review

**Status:** Passed; mandatory exit gates verified
**Date:** 2026-07-26
**Scope:** PI-04 scope completeness, determinism, save safety, presentation truthfulness, and package isolation

## Review question

Could Phase 2 appear complete while only adding a larger demo—without sustained liveness, resource contention, meaningful specialization, recoverable weather/incidents, transient save safety, usable player controls, or cooked/offline proof?

## Risks and required countermeasures

### A scripted happy path could hide deadlock or insolvency

A short visit cannot validate recurring demand, construction costs, wages, staff/vehicle contention, or recovery. The exit gate therefore requires three independently seeded 14-operating-day fixtures for GA, flight school, and charter. Each fails on insolvency, stalled authoritative work, impossible reservation, unexplained conflict, invalid state, or non-deterministic checksum.

### Phase 2 could duplicate Phase 1 authority

Adding a separate living-airport balance or facility truth would make save/load and UI disagreement inevitable. Phase 2 must use typed integration commands against the Phase 1 owner for Credits, Airport Points, transactions, identity, and starter facilities. Tests must reject mismatched or duplicated authority.

### Concurrency could be cosmetic

Multiple sprites are insufficient evidence. Tests must prove overlapping runway, taxi, stand, tow, vehicle, staff, and service reservations; exclusive resources cannot double-book; waits expose stable cause and remedy; bounded queues eventually progress when constraints clear.

### Specializations could be renamed copies

GA, flight school, and charter require distinct traffic cadence, aircraft roles, service demand, tenant evidence, risks, and economics. Scenario assertions compare those differences and show that selecting one path does not secretly erase the others.

### Weather and incidents could become arbitrary punishment

Forecasts use named deterministic streams and publish operational categories before consequences where appropriate. Incidents expose cause, readiness, response, containment, reopening, cost, and rating effects. Recovery is bounded and explicit. Seed replay and boundary saves must reproduce the outcome.

### Save coverage could omit transient ownership

Round-trip at rest is not enough. Required saves occur during project delivery/build/inspection, aircraft taxi and tow, concurrent turnaround, weather transition, deicing, incident dispatch/containment, and reopening. Resumed checksums and events must match uninterrupted continuation.

### Data assets could exist in editor but disappear from Shipping

Runtime strings do not establish cooking dependencies. Production root widgets, component assets, sprites, textures, and Phase 2 definitions require serialized or constructor-time cooker-visible references. The editor audit, cooked-package inspection, Shipping exercise, and forbidden-reference scan all fail closed.

### The approved UI could regress under feature density

Phase 2 cannot solve density by shrinking text, restoring square prototype panels, or covering the world. New surfaces must reuse the Phase 1.5 tokens and components, switch to compact drawers at high scale, retain a world-dominant viewport, preserve visible mouse/focus behavior, and pass equivalent-state visual comparison.

### Headless controls could mask an unusable game

Every exit-critical command must be reachable through the normal player UI. Automation may drive semantic controls, but debug console commands and fixture-only mutations cannot count as usability evidence.

### External or 3D art could slip into the content tranche

The plan explicitly forbids importing the external aircraft directory and requires internally authored fictional 2D assets unless a separate approved manifest exists. The content audit rejects unreviewed source paths, missing provenance, required 3D candidates, and unstable content IDs.

### Editor tooling could leak into runtime

Unreal MCP, Python, editor modules, RemoteControl, tests, and Toolset remain authoring dependencies only. Shipping receipt, file, text, and dependency scans must report zero forbidden matches.

## Exit judgment

The mandatory 14-day, transient-save, rendered-UI, cooked-asset,
offline-package, and forbidden-dependency evidence now exists.

The review caught its intended failure mode during closeout: the first clean
Shipping build executed successfully but its IoStore inventory contained none
of the 33 new definition assets. The Asset Manager scan was therefore
insufficient as a cooker root. `/Game/Phase2` is now explicitly always-cooked,
the corrected container has 300 packages, and the pipeline compares all 33
source asset names with the direct IoStore inventory. A working executable
alone can no longer satisfy the gate.

No implementation-level issue remains open. The Phase 7 four-hour physical-tier
soak and formal unassisted tester protocol remain deferred and are not
misreported as Phase 2 evidence.
