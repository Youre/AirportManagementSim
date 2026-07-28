# CT-05: Visual Content Requirements

**Status:** Approved implementation baseline  
**Owner:** Product, visual design, and presentation engineering  
**Approved:** 2026-07-26  
**References:** VA-01 through VA-07  

## Purpose

This specification converts the approved concept-art set into testable visible
content requirements. It closes the gap between a consistent color/component
theme and a complete player-facing presentation.

The written gameplay specifications remain authoritative for behavior, values,
timing, geometry, and content rights. CT-05 is authoritative for which
player-facing visual structures, world cues, inspectors, and overlays must
exist when their owning gameplay is available.

## Outcome and non-goals

The completed presentation must let a player understand routine airport
activity by looking at the world and its adjacent UI, rather than reconstructing
the operation from summary prose.

CT-05 does not:

- add Phase 5 cargo behavior or Phase 6 major-airport simulation;
- copy incidental concept-art values, brands, aircraft, or geometry;
- make Figma, screenshots, widgets, or sprites authoritative simulation state;
- authorize 3D assets, perspective cameras, runtime string asset loading, or
  unreviewed external art;
- require pixel-perfect reproduction of generated images.

Later-phase destinations may appear in progression as visibly locked,
descriptive destinations. They cannot pretend that unavailable gameplay is
implemented.

## Status vocabulary

Every requirement uses one implementation status:

- **Present:** complete player-facing content exists and is exercised.
- **Text-only:** authoritative information exists, but the world or composed
  visualization does not communicate it.
- **Schematic:** the correct visual layer exists using provisional geometry or
  silhouettes, but it has not reached the required content density.
- **Absent:** no corresponding player-facing component exists.
- **Future-locked:** the destination is intentionally visible but its gameplay
  remains owned by a later phase.

## Shared requirements

| ID | Required visible outcome | Acceptance evidence |
| --- | --- | --- |
| VC-COM-01 | The airport world is strict top-down 2D and is the largest continuous normal-scale surface. | Primary 100–150% captures retain at least 60% continuous world area except full-screen planning/capability surfaces. |
| VC-COM-02 | Persistent identity, economy, rating/progression, date/time, weather, pause, and speed are glanceable without opening a drawer. | Equivalent-state capture and widget-content test. |
| VC-COM-03 | Rounded navy surfaces, cyan selection, amber caution, coral critical, green complete, and off-white text use the accepted Riverbend component language. | Component gallery and theme test. |
| VC-COM-04 | Operational state never relies on color alone. Icons, patterns, shapes, or concise text reinforce every warning and state. | Component gallery, capture review, and state-encoding test. |
| VC-COM-05 | Important UI explanations have a visible world counterpart when the condition is spatial: selection, route, queue, closure, conflict, service, or incident. | Same-state world/UI capture and presentation test. |
| VC-COM-06 | Routine detail stays in contextual inspectors. Persistent rails do not become diagnostic text dumps. | Copy-density worksheet and capture review. |
| VC-COM-07 | Every visible icon, prop, aircraft, person, vehicle, marking, and texture is a reviewed asset or deterministic project-owned graphic—not a text symbol or placeholder box. | Content manifest, provenance review, and Shipping inventory. |
| VC-COM-08 | The complete content hierarchy survives 100%, 125%, 150%, 175%, and 200% application scale. | Five-scale matrix with compact drawers at 175–200%. |
| VC-COM-09 | Player-visible content derives from immutable query/state data and cannot mutate simulation through presentation coordinates. | Mapping and isolation automation. |
| VC-COM-10 | Visual acceptance uses an equivalent gameplay state, zoom, and scale. A differently scoped screenshot cannot count as parity evidence. | Combined board metadata and review worksheet. |

## VA-01 — airport overview and HUD

| ID | Required visible outcome | Current status |
| --- | --- | --- |
| VC-OVR-01 | A mature overview shows the complete unlocked airport: runway/taxi/stands, terminal or hut, access, aircraft, vehicles, people, and current work. | Present |
| VC-OVR-02 | A compact left tool rail exposes build, zones/routes, schedules, staff, and overlays through icon-plus-label tools. | Present |
| VC-OVR-03 | A right activity rail separates alerts, active flights, and construction/projects. | Present |
| VC-OVR-04 | The top status bar satisfies VC-COM-02 without crowding or multi-line compression. | Present |
| VC-OVR-05 | Selecting an aircraft or facility opens a bottom inspector with identity art, readiness, capacities, services, and valid contextual actions. | Present |
| VC-OVR-06 | Unlocked GA, passenger, service, and landside activity can coexist visibly at management zoom. | Present |
| VC-OVR-07 | Empty-airport creation and mature-airport overview are separate validated states. Neither may substitute for the other. | Present |

## VA-02 — build mode and construction

| ID | Required visible outcome | Current status |
| --- | --- | --- |
| VC-BLD-01 | Entering build mode changes the world into an explicit proposal surface before commitment. | Present |
| VC-BLD-02 | A categorized construction palette exposes lifecycle-valid runway, taxiway, stand, route, marking, and facility choices. | Present |
| VC-BLD-03 | Proposed valid geometry uses cyan outline plus a non-color pattern and selection handles. | Present |
| VC-BLD-04 | Conflicts are localized in coral plus a distinct pattern and concise label. | Present |
| VC-BLD-05 | Affected-only closure is drawn on the precise operational area; unaffected operations remain visibly open. | Present |
| VC-BLD-06 | A validation rail shows validity, conflict, closure, cost, materials, workforce, compatibility, and capability gain. | Present |
| VC-BLD-07 | Confirmation is available only after the proposal and consequences are understandable. | Present |
| VC-BLD-08 | Committed work remains spatially visible through delivery, construction, inspection, completion, cancellation, and refund states. | Present |

## VA-03 — terminal and passenger flow

| ID | Required visible outcome | Current status |
| --- | --- | --- |
| VC-TRM-01 | The terminal is a readable cutaway with distinct room boundaries, doors, queue furniture, seating, desks, baggage equipment, and amenities. | Present |
| VC-TRM-02 | Gates, aircraft, apron, curb, parking, road access, buses, taxis, and other unlocked landside modes are visible around the terminal. | Present |
| VC-TRM-03 | Departure flow is a continuous directional path from landside arrival through bag drop, security, lounge, and gate. | Present |
| VC-TRM-04 | Arrival flow is a separate continuous directional path through arrivals, reclaim, and landside exit. | Present |
| VC-TRM-05 | Controlled boundaries and security transitions use a spatial boundary plus icon/text evidence. | Present |
| VC-TRM-06 | Departing and arriving baggage is visibly connected to passengers, make-up, aircraft, reclaim, and exceptions. | Present |
| VC-TRM-07 | Accessible routing is visible end to end and distinct by pattern as well as color. | Present |
| VC-TRM-08 | Passengers, parties, bags, staff, and vehicles use bounded pooled 2D proxies with recognizable silhouettes. | Present |
| VC-TRM-09 | The selected-party inspector shows identity, party members, flight, step, compact needs, route, and time confidence. | Present |
| VC-TRM-10 | Terminal tools and the route legend remain available without obscuring the cutaway. | Present |

## VA-04 — flight planning and timetable

| ID | Required visible outcome | Current status |
| --- | --- | --- |
| VC-TIM-01 | The dominant planning surface is a true time-based grid with day and time geometry; gate/stand ownership is visible where relevant. | Present |
| VC-TIM-02 | Flight cards are positioned and sized by their authoritative slot rather than displayed as uniform summaries. | Present |
| VC-TIM-03 | Every flight card reveals fictional operator, flight, aircraft, time, gate, and status at a glance. | Present |
| VC-TIM-04 | The selected contract card includes operator identity art, aircraft silhouette, route/frequency, capacity, and reward evidence. | Present |
| VC-TIM-05 | The selected-flight inspector keeps runway, stand, service, terminal, baggage, weather, connection, border, and transport evidence adjacent to the decision. | Present |
| VC-TIM-06 | Conflicts and locked-horizon risk are localized on the affected card/cell using pattern plus text. | Present |
| VC-TIM-07 | Weather advisories visibly identify the affected time window and flights. | Present |
| VC-TIM-08 | Scheduling, gate-change review, and confirmation use one clear contextual action area. | Present |
| VC-TIM-09 | The player can inspect the entire seven-day authored timetable without mistaking a day summary for a time grid. | Present |

## VA-05 — aircraft turnaround

| ID | Required visible outcome | Current status |
| --- | --- | --- |
| VC-TRN-01 | The selected aircraft is fully visible and is the visual anchor, occupying roughly 22–40% of the close-operations world width. | Present |
| VC-TRN-02 | Assigned service vehicles, workers, and equipment appear at recognizable safe positions around the aircraft. | Present |
| VC-TRN-03 | Safety/exclusion zones and vehicle approach paths are visible without relying on color alone. | Present |
| VC-TRN-04 | A task timeline shows order, concurrency, dependencies, waiting, active, complete, and blocked work. | Present |
| VC-TRN-05 | Automatic dispatch remains explicit; player interaction changes priority or safe assignment rather than manually driving. | Present |
| VC-TRN-06 | The selected-aircraft inspector includes operator/aircraft identity, gate/stand, schedule, capacity, and predicted readiness. | Present |
| VC-TRN-07 | Task progress and vehicle movement update from authoritative service state at bounded presentation frequency. | Present |
| VC-TRN-08 | The close-operations camera and normal management camera are separately validated. | Present |

## VA-06 — weather and incident response

| ID | Required visible outcome | Current status |
| --- | --- | --- |
| VC-INC-01 | Current rain, wind, visibility, temperature, surface, and operational limits are scanable in a weather rail. | Present |
| VC-INC-02 | The forecast shows time, conditions, wind, visibility, confidence, and operational implication. | Present |
| VC-INC-03 | Weather visibly affects the world through reviewed rain, wet-surface, visibility, and lighting treatments without obscuring actions. | Present |
| VC-INC-04 | The affected aircraft/area is a clear world focal point. | Present |
| VC-INC-05 | Closure is spatially bounded and unaffected operations are visibly distinguishable. | Present |
| VC-INC-06 | Emergency route, responding vehicles, and protected area are visible in the world. | Present |
| VC-INC-07 | The response rail shows alerted, dispatched, protected, stabilized, investigating, and recovered lifecycle states. | Present |
| VC-INC-08 | Cause, consequence, response choices, and prevention/recovery are concise and adjacent. | Present |
| VC-INC-09 | Human outcome remains abstract and age-appropriate; continued safe operations are explicit. | Present |
| VC-INC-10 | Captioned radio and status changes remain visible over the weather treatment. | Present |

## VA-07 — progression and specializations

| ID | Required visible outcome | Current status |
| --- | --- | --- |
| VC-PRG-01 | One capability map gives GA, flight school, charter, cargo, passenger, and mixed airports equal visual weight. | Present |
| VC-PRG-02 | The current airport sits at the center and connects non-exclusively to every specialization. | Present |
| VC-PRG-03 | Established, Regional, Advanced, and Major capability bands are visible; Major is not presented as an ending. | Present |
| VC-PRG-04 | Exactly three suggested objectives are visible with icon, progress, and concise reward/outcome. | Present |
| VC-PRG-05 | Selecting a path opens requirements, current evidence, next capability, and rewards. | Present |
| VC-PRG-06 | The surface explicitly explains that paths can be combined. | Present |
| VC-PRG-07 | No visual hierarchy funnels every player toward passenger operations. | Present |
| VC-PRG-08 | Cargo, mixed, or Major destinations appear as honest future-locked nodes until their owning phase is complete. | Future-locked |
| VC-PRG-09 | Each path uses a recognizable project-owned world thumbnail and icon family. | Present |

## Required reusable components

| Component ID | Component | Primary requirement owners |
| --- | --- | --- |
| VC-C01 | Persistent status bar | VC-COM-02, VC-OVR-04 |
| VC-C02 | Icon tool rail | VC-OVR-02, VC-TRM-10 |
| VC-C03 | Activity rail | VC-OVR-03 |
| VC-C04 | Selected-entity inspector | VC-OVR-05, VC-TRN-06 |
| VC-C05 | Build palette | VC-BLD-01, VC-BLD-02 |
| VC-C06 | Placement and conflict overlay | VC-BLD-03 through VC-BLD-05 |
| VC-C07 | Construction validation/summary | VC-BLD-06 through VC-BLD-08 |
| VC-C08 | Terminal cutaway prop kit | VC-TRM-01, VC-TRM-02 |
| VC-C09 | Passenger/baggage/route world layers | VC-TRM-03 through VC-TRM-08 |
| VC-C10 | Selected-party inspector | VC-TRM-09 |
| VC-C11 | Time-based timetable grid | VC-TIM-01, VC-TIM-02, VC-TIM-09 |
| VC-C12 | Contract and flight identity cards | VC-TIM-03, VC-TIM-04 |
| VC-C13 | Flight feasibility inspector | VC-TIM-05 through VC-TIM-08 |
| VC-C14 | Close-operations turnaround surface | VC-TRN-01 through VC-TRN-03 |
| VC-C15 | Service task timeline | VC-TRN-04, VC-TRN-05, VC-TRN-07 |
| VC-C16 | Weather and forecast rail | VC-INC-01 through VC-INC-03 |
| VC-C17 | Incident world layer | VC-INC-04 through VC-INC-06 |
| VC-C18 | Incident lifecycle and recovery inspector | VC-INC-07 through VC-INC-10 |
| VC-C19 | Capability map | VC-PRG-01 through VC-PRG-03 |
| VC-C20 | Suggested objectives | VC-PRG-04 |
| VC-C21 | Specialization inspector | VC-PRG-05 through VC-PRG-09 |
| VC-C22 | Project-owned icon/prop/people/vehicle asset kit | VC-COM-04, VC-COM-07 |

## Implementation boundary

- `AMSimSimulation` remains the authority for commands, records, timing,
  conflicts, services, passengers, baggage, weather, incidents, and
  progression.
- `AMSimGameplay` continues to resolve definitions and orchestrate save/load.
- `AMSimUI` owns presentation-only view models, composed UMG/CommonUI surfaces,
  camera modes, pooled Paper2D layers, and visual selection state.
- Blueprint assets may own composition, brushes, spacing, and animation.
  Native presenters own query mapping, commands, cooker-visible references, and
  safe fallback.
- Figma is a supplemental design/validation artifact. Requirement IDs and
  implementation truth live in this repository.

## Acceptance gate

CT-05 is complete only when:

1. VC-C01 through VC-C22 exist and are mapped to the requirement IDs above;
2. every requirement is `Present` or an explicitly approved `Future-locked`
   destination;
3. all seven references have genuinely equivalent 1920×1080 comparison states;
4. the world communicates selection, routes, services, conflicts, and incidents
   without depending on explanatory prose;
5. the five-scale matrix, component gallery, visible-mouse checks, presentation
   isolation tests, full Phase 1–4 automation, clean Shipping package, content
   inventory, and offline smoke test pass.
