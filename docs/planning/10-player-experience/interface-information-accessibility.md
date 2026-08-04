# Interface, Information, and Accessibility

**ID:** PX-03  
**Status:** Approved  
**Depends on:** PX-01, PX-02, TS-06, all gameplay query contracts  
**Integration phase:** Phase 0 shell, completed incrementally

## Outcome

The interface explains a complex airport with short text, authentic terminology, consistent visual grammar, and drill-down detail. A child can identify what needs attention, why it happened, and what action can improve it.

## Information architecture

The root HUD contains:

- top bar: credits, airport points, rating, game date/time, weather summary, simulation speed;
- left tool rail: build, demolish, zones, schedules, tenants, staff, overlays;
- right activity rail: alerts, flights, projects, achievements;
- bottom context panel: selected entity or facility;
- modal stack: save/load, region selection, settings, destructive confirmations only.

In the starter-airfield journey, Build, Schedule, Staff, and Overlays are real
rail destinations rather than disabled previews. Schedule opens a bounded
first-visit operations card with the exact arrival, wall-clock countdown,
arrival-to-departure timeline, current/next state, and an explicit statement
that aircraft movement and routine ground service are automatic. Scheduling
pauses the simulation so a previously selected fast construction speed cannot
skip the arrival. The player then chooses `Watch at 1x` or `Advance to arrival`;
the latter may use 8x only until the first visible inbound state and then
returns to 1x. During the visible operation, `Continue at 4x` provides a
deliberate faster observation path.

During normal map play, the left tool rail uses one consistent icon family and
keeps its labels collapsed. Hover or keyboard focus reveals a short label in a
rounded flyout beside the icon without resizing the rail or shifting the map.
Every icon keeps a tooltip and at least a 44 px-equivalent target; unavailable
actions remain explainable on hover/focus even though activation is disabled.
Catalogs, settings, and compact-drawer alternatives continue to pair icons with
persistent text.

Panels are layered through CommonUI. Only one primary management panel occupies the workspace at a time. Inspect panels may remain pinned while the player opens a primary panel.

## Inspectable entities

Every player-visible aircraft, passenger, staff team, vehicle, bag exception, tenant, flight, facility, project, and incident has an inspect query.

The selected-object panel follows a common order:

1. identity and current status;
2. current task and destination;
3. blocking reason, if any;
4. relevant needs, condition, compatibility, or workload;
5. recent history;
6. available player actions.

The panel never exposes internal class names, object paths, array indices, or debug-only state.

## Alerts

Severity levels:

- information: no action required;
- advisory: improvement or upcoming constraint;
- warning: service or schedule at risk;
- critical: safety event, closure, or irreversible outcome in progress.

Every warning and critical alert includes:

- affected object;
- plain cause;
- expected consequence;
- time until consequence when applicable;
- at least one navigation or corrective action.

Duplicate alerts aggregate by cause and zone. Dismissal acknowledges the current instance; it does not suppress future critical events. Alert history is retained for 30 game days.

## Overlays

Required overlays:

- ownership and purchasable land;
- construction validity and work zones;
- network connectivity;
- aircraft compatibility;
- taxi and runway reservations;
- passenger flow and congestion;
- secure zones;
- baggage connectivity and load;
- staff coverage;
- service response;
- road and rail access;
- weather and runway suitability;
- noise is not simulated as a community constraint and therefore has no management overlay;
- rating and satisfaction drivers.

Overlay legends remain visible and label every color with a symbol or pattern.
The Phase 1 rail exposes Airfield, Connections, and Activity modes. Connections
emphasizes the runway-to-gate movement graph and road access; Activity mutes
infrastructure so aircraft, crews, and service markers remain prominent. Each
mode also has a text description, so its meaning is not color-only.

## Readability

- Minimum body text: equivalent of 18 px at 1080p and 100% UI scale.
- UI scale settings: 100%, 125%, 150%, 175%, 200%.
- Essential information is not communicated by color alone.
- Icons require text labels in catalogs and settings.
- Critical messages use short sentences and define uncommon non-aviation terms.
- Aviation terminology remains authentic and can expose contextual definitions.
- Captions are enabled by default for radio and announcements.
- Caption speaker, aircraft callsign, and message category are visually distinct.
- Flashing effects have a reduced-motion alternative.
- Camera shake is disabled by default.

## Error states

Empty, loading, unavailable, and failed states are explicit. A panel cannot silently show stale data after a save load or entity deletion. If a pinned entity no longer exists, the panel shows its last identity and why it disappeared.

## Visual references

[VA-01 Airport overview and HUD](../30-content-and-assets/concept-art/01-airport-overview-hud.png) is the primary information-architecture reference. [VA-03 Terminal and passenger flow](../30-content-and-assets/concept-art/03-terminal-passenger-flow.png) demonstrates overlay legends and a named-party inspect panel. [VA-06 Weather and incident response](../30-content-and-assets/concept-art/06-weather-incident-response.png) demonstrates caption, cause, consequence, severity, and corrective action in one operational state.

Implementation validation must preserve hierarchy and redundant icon/pattern/text encoding, not the images' exact dimensions, font, or incidental copy.

## Acceptance criteria

- A warning can be traced from alert to affected object and corrective tool in no more than two actions.
- All required overlays have legends and non-color encodings.
- Every radio call can be understood with audio muted.
- UI remains usable at every supported scale at 1920×1080.
- Entity panels update from query snapshots and survive presentation-proxy replacement.
- No player-facing panel exposes implementation identifiers or debug text.
