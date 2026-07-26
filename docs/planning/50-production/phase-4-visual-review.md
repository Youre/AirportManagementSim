# Phase 4 Visual Review

**Status:** Passed for the Phase 4 in-scope presentation
**Reviewed:** 2026-07-26
**References:** VA-04 and VA-06

## Evidence set

The deterministic regional-week fixture was captured at 1920 x 1080 and 100%,
125%, 150%, 175%, and 200% application scale. The equivalent-state comparison
boards are:

- `AMSim/Saved/Phase4/Comparisons/va04-phase4-comparison.png`;
- `AMSim/Saved/Phase4/Comparisons/va06-phase4-comparison.png`.

The implementation captures are
`AMSim/Saved/Phase4/va04-regional-timetable.png` and
`AMSim/Saved/Phase4/va06-regional-incident.png`. The source references are
`docs/planning/30-content-and-assets/concept-art/04-flight-planning-timetable.png`
and
`docs/planning/30-content-and-assets/concept-art/06-weather-incident-response.png`.
Generated captures remain verification artifacts, not runtime assets.

## VA-04 details deliberately incorporated

- deep-navy rounded chrome around a dominant planning surface;
- a compact recurring-contract rail, seven-day timetable, and adjacent
  selected-flight evidence rail;
- exact arrival/departure, gate, operator, and aircraft identity on every
  flight card;
- cyan structure, green complete/readiness state, amber high-risk state, coral
  critical state, and text/checkmark redundancy;
- runway, stand, services, connections, bags, border, transport, weather, and
  tenant evidence adjacent to the decision;
- a two-step warning/confirmation path for the locked-horizon gate change;
- compact day-focused cards and bounded scrolling rails at 175-200%.

The implementation deliberately shows all three accepted fictional operators
and the complete 21-flight week instead of copying the concept's incidental
offer, times, rewards, or column count.

## VA-06 details deliberately incorporated

- weather/current-conditions rail, world-dominant center, incident lifecycle
  rail, caption, and bottom cause/remedy tray;
- rain, wind, visibility, runway surface, six-hour confidence, and approach
  limit evidence connected to the incident;
- localized Paper2D closure, emergency-route, and response-vehicle layers
  owned by the world presenter;
- explicit alerted/dispatched/protected state, affected-runway closure, and
  continued-operation statement;
- hold, divert, tow, protect, cause-review, and recovery actions exposed only
  when valid;
- abstract `No injuries` outcome and no depiction of human harm.

## Highest-impact mismatches and iteration

1. The first incident cards retained full query prose in narrow rails and
   clipped at high UI scale.
2. The first timetable cards did not expose aircraft class without consulting
   the contract rail.
3. The concept has bespoke illustrated airport, icon, vehicle, and aircraft
   art; the implementation uses the accepted project-owned schematic Paper2D
   language.

The bounded iteration targeted information density and status legibility.
Weather, incident, selected-flight, feasibility, connection, border, transport,
and tenant copy now has compact data-derived variants; every timetable card
shows an aircraft code; the compact footer is taller; and the brand shortens
without losing airport identity. Recaptured 175% and 200% states have no clipped
critical status or action.

## Intentional differences

1. VA-04 uses time rows and a smaller illustrative flight set. The authored
   fixture uses seven day columns with three exact services per day so the
   whole deterministic week remains inspectable.
2. VA-06 has production-density airport art. Phase 4 preserves the established
   schematic terminal/world and isolates richer props, icons, aircraft, and
   emergency vehicles as later art-production work rather than faking them.
3. Concept values and real-looking operators are illustrative. Runtime copy is
   derived from the Phase 4 fixture and uses fictional operators and aircraft.

These differences do not change the reference hierarchy, player task, warning
semantics, or recovery path. There are no P0-P2 visual defects in the supported
scale matrix.
