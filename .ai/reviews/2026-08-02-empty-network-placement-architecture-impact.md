# Empty network placement overhaul — architecture impact

Date: 2026-08-02

Status: approved for implementation

## User-visible outcome

The starter parcel opens empty. The player draws a runway and taxiway directly
on the map using start and end circles, places the remaining required facilities,
and sees the real completed surfaces. Taxiways may legally meet or cross the
runway. Runway numbers and markings derive from the placed runway heading.

## Non-goals

- No curved or multi-segment taxiway graph, parallel-runway suffixes, displaced
  thresholds, magnetic-variation simulation, demolition, or Phase 2–7 content.
- No change to the starter plan price, construction stages, or schema 8 save
  format.
- No simulation positions may be derived from presentation components.

## Decisions

1. `FStarterPlanProposal` remains the authoritative persisted geometry contract.
   A presentation-only completion mask distinguishes the empty editor from the
   hidden seed values required by the fixed schema.
2. Normal proposal entry clears every completion bit. The existing deterministic
   visual-proof command may intentionally mark its authored fixture complete.
3. The proposal presenter owns pointer state and completion state. It renders
   only a start circle before the first point, a thin centerline and end circle
   during the gesture, and the full surface after the second endpoint.
4. `ValidateStarterPlan` becomes orientation-independent. Taxi/runway connection
   is satisfied by segment intersection or endpoint proximity to the runway
   segment; taxi/stand and road/stand rules remain authoritative.
5. Runway designators are a pure geometry derivation shared by presentation and
   tests: north-up map heading rounded to the nearest ten degrees, `01`–`36`,
   with the reciprocal end offset by 18.
6. The Paper2D presenter maps proposal coordinates to the world and transforms
   runway, taxiway, and road sprites to their actual center, length, and angle.
   It never feeds positions back into simulation.
7. No runtime string asset loads, new external art, Blueprint authority, or save
   migration is introduced.

## Compatibility and rollback

- Existing schema 1–8 saves remain readable because proposal fields do not
  change.
- Existing completed starter plans render from their stored geometry.
- The authored default proposal remains a deterministic test fixture, not a
  visible or automatically accepted player layout.
- Rollback is limited to the Phase 1 validator, proposal presenter, world
  transform helper, focused tests, and documentation.
