# First-visit control loop — preliminary adversarial review

Date: 2026-08-03

## Failure cases and controls

- **Build appears editable after funding:** the post-funding page says the
  starter build is committed and reports status/network evidence. It provides
  no commit or placement control.
- **Schedule remains a dead placeholder:** the rail action is enabled once an
  airport exists and always opens prerequisite, slot-selection, active-flight,
  or completion guidance.
- **Acceleration skips the visible visit:** `Advance to arrival` uses the
  existing 8x command and returns to 1x at the first visible inbound state.
- **Scheduling is still rejected immediately:** the first offered slot is the
  simulation subsystem's recommended valid slot, without an extra arbitrary
  five-minute delay.
- **Player cannot tell whether intervention is required:** the active page
  explicitly states that arrival, taxi, turnaround, and departure are
  automatic, and names the next state.
- **Progression completes with no next action:** completion points to the
  user-facing `Start Living Airport` action rather than internal phase jargon.
- **Overlay button is cosmetic:** Airfield, Connections, and Activity modes
  produce distinct world-presentation state and include text descriptions so
  meaning is not color-only.
- **Menus obscure the world:** the hub is a bounded rounded panel without a
  dimming full-screen backdrop and leaves the majority of the map visible.
- **Several panels fight for focus:** opening the operations hub closes Staff,
  construction placement, and the schedule picker as appropriate; closing it
  restores ordinary map interaction.
- **Saved determinism changes:** no new domain state or serialization is
  added; tests cover presentation derivation and existing Phase 1 replay.

## Preliminary verdict

No blocking architecture or safety issue. Implementation must prove the rail
actions, derived states, speed return boundary, overlay modes, and regression
suite before acceptance.
