# Starter Network Context Adversarial Review

Date: 2026-08-02
Status: Approved with controls below

## Failure modes and controls

1. **A segment touches a runway but not a gate.** Validation must report the
   missing gate path and highlight the nearest unconnected network endpoint.
2. **A segment touches a gate through another taxiway.** Graph traversal, not
   endpoint-only comparison, must accept the chain.
3. **One valid chain hides a disconnected extra segment.** Every retained
   segment must belong to the runway/gate component or commitment remains
   blocked with the orphan segment identified.
4. **Crossing geometry is mistaken for a disconnection.** Proper and collinear
   segment intersections divide the graph and remain legal.
5. **The player cannot see what counts as a connection.** Taxi mode must
   highlight both gate ports, the runway surface, and existing taxi segments;
   pointer snapping must use the same tolerance as validation.
6. **Smaller circles become unusable.** Visual circles shrink and lose
   `START`/`END` copy, while map-space hit tolerance remains generous and hover
   or selection provides a non-color outline change.
7. **The fixed terminal silently becomes Phase 3 passenger gameplay.** It is a
   starter operations shell and aircraft gate anchor only; baggage, security,
   passenger rooms, and terminal expansion remain Phase 3.
8. **Road placement still blocks the plan.** Road geometry is optional and has
   no named connection prerequisite. Invalid bounds may be disclosed, but a
   missing or isolated road cannot block runway-to-gate readiness.
9. **A random road grants an unexplained bonus.** The bonus applies only when a
   non-zero segment passes near the construction network or starter terminal;
   UI states the percentage and the no-road walking baseline.
10. **Schema migration corrupts old packages.** The new segment array is read
    only for schema 9. Schema 8 and earlier deserialize their original byte
    layout before migration creates one taxi segment from the legacy fields.
11. **World rendering drops branches.** A bounded pool renders every stored
    segment up to the proposal limit and tests assert count, transform, and
    visibility reuse.
12. **Later systems still target an unconnected Gate A.** New plan
    normalization selects the first gate reached by the accepted network as
    the authoritative Phase 1 stand center.

## Required proof

Compilation, focused graph/UI/migration tests, the complete automation suite,
clean Shipping cook, forbidden dependency scan, and a real Windows replay must
all pass. The replay must use at least two taxiway segments, with one segment
joining another before reaching a gate.
