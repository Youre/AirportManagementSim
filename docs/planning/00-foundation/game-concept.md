# Initial Game Concept

**Status:** Draft  
**Owner:** Project team  
**Last updated:** 2026-07-25  
**Source:** `ideas.txt`, local reference screenshots, reusable-aircraft inventory, and comparable-game research

## Concept statement

Build a living airport from a rough grass airstrip into a high-capacity international hub in a single-player, 2D top-down management simulation. The player designs airside and landside infrastructure, watches real aircraft and individual travelers use it, makes operational decisions under changing weather and demand, and reinvests the resulting revenue and reputation into the next class of aviation.

The experience should combine the clarity and creativity of a builder with aviation rules that visibly matter. A longer paved runway is not merely a bigger number: it changes which aircraft can arrive, which services they need, what traffic they create, how much the airport can earn, and what can go wrong operationally.

## Player fantasy

The player is the owner-operator and long-term steward of an airport. They begin with land, a road connection, limited capital, a box truck of supplies, and a bus carrying the first construction crew. Their first meaningful achievement is not placing a decorative building; it is creating a functioning airfield and seeing the first named aircraft safely land, park, exchange people or cargo, pay its fees, and depart.

Over time, the player moves between four complementary roles:

- **Planner:** purchases land and lays out runways, taxiways, stands, terminals, roads, parking, rail, and support facilities.
- **Operator:** opens or closes the airport, resolves capacity constraints, responds to weather, and keeps aircraft and passengers moving.
- **Business manager:** sets priorities, signs tenants and airlines, hires staff, controls recurring costs, and decides which growth opportunities justify investment.
- **Airport observer:** follows individual aircraft, tail numbers, passengers, workers, and vehicles as the airport's systems create visible stories.

## Design pillars

### 1. Start small; make every upgrade legible

The grass-strip opening is a core identity, not a short tutorial to rush through. Each expansion should produce an observable change in traffic, procedures, workload, services, and earning potential.

### 2. Aviation authenticity with readable consequences

Aircraft use plausible runway requirements, traffic patterns, taxi routes, parking sizes, tail numbers, and weather rules. Wind influences runway selection; visibility and ceilings influence VFR, special VFR, and IFR operations; lighting and navigation aids extend safe operating conditions. The game communicates why a flight can or cannot operate without requiring professional aviation knowledge.

### 3. The airport is a connected system

Airside, terminal, ground transport, staffing, tenants, emergency coverage, and passenger satisfaction are not isolated build menus. Growth in one area creates requirements elsewhere. Connections and capacity should be visible enough that players can diagnose problems and improve their design.

### 4. Watchable miniature stories

Aircraft, passengers, construction workers, buses, taxis, rental cars, service vehicles, and trains should remain understandable at normal play speed. Clicking an entity reveals useful context: who it is, what it is trying to do, its route or flight, current needs, delays, and satisfaction.

### 5. Player-directed growth

Objectives guide rather than dictate. The player can favor general aviation, passenger airlines, cargo, charter operators, flight schools, events, or a mixed hub. Different strategies should change the shape and rhythm of the airport while sharing the same coherent simulation.

## Core gameplay loop

1. **Observe:** inspect demand, traffic, finances, weather, satisfaction, and bottlenecks.
2. **Plan:** choose an operational or growth goal and lay out the necessary facilities.
3. **Close or constrain operations when required:** major runway and taxiway work has a real operational cost.
4. **Construct:** supplies and workers arrive physically and complete the work over time.
5. **Operate:** reopen or expand service; aircraft, passengers, tenants, and vehicles use the new capacity.
6. **Diagnose and optimize:** correct pathing, queues, service coverage, staffing, scheduling, and weather resilience.
7. **Earn and progress:** collect aviation and commercial revenue, complete objectives, gain experience and reputation, unlock capabilities, and purchase more land.

This loop should work at multiple scales. Early play may revolve around one arrival and a few parking stands. Late play applies the same reasoning to intersecting traffic flows, multiple terminals, airline contracts, cargo operations, rail links, and weather disruptions.

## Opening experience

The new game begins on a mostly grass landscape with a single public road entering from the top edge of the map. The initial budget is deliberately tight; the `$30` figure in `ideas.txt` is a placeholder for later balancing.

A delivery truck and worker bus introduce the construction model. The player chooses a starter runway, taxi connection, parking position, and essential building package that fits the budget. Grass infrastructure makes small general aviation practical but limits traffic volume, aircraft weight, weather reliability, and service revenue.

Opening the airport is an explicit player action. The first flights use real small single- and twin-engine aircraft where rights and data allow. Each aircraft receives a persistent identity for the visit, including type, tail number, origin/destination, operating purpose, occupants, service needs, and departure intent. Some visitors turn quickly; others leave the aircraft parked while a pilot or passengers travel off-airport.

The opening succeeds when the player understands the complete airport cycle:

**arrival → runway → taxi → stand/hangar → people or cargo exchange → optional service → payment → taxi → departure**

## Progression model

Progression has four linked currencies:

- **Money** pays for construction, land, staff, maintenance, utilities, services, and disruption recovery.
- **Experience** comes from objectives and sustained operations; it unlocks knowledge and tool access.
- **Airport rating** reflects passenger and tenant outcomes; it affects demand, contract quality, and pricing power.
- **Operational capability** is earned through actual infrastructure and staffing. A level unlock does not make an airport jet-capable unless runway, stand, terminal, rescue, and service requirements are also met.

| Stage | Airport identity | Typical aircraft and activity | New management emphasis |
| --- | --- | --- | --- |
| 1 | Local grass airfield | Light GA singles/twins, training, private visits | Basic construction, runway use, parking, weather awareness, opening/closing |
| 2 | Developed local airport | Advanced GA, bush aircraft, small turboprops, charter | Paved surfaces, lights, fuel, hangars, staff, rental cars, simple ATC |
| 3 | Regional airport | Regional turboprops and jets, scheduled passengers, light cargo | Security, schedules, baggage, airline contracts, terminal flow, rescue coverage |
| 4 | National airport | Narrow-body jets, multiple airlines, cargo and events | Gate allocation, deicing, tenant mix, traffic peaks, larger parking and transport |
| 5 | International hub | Wide-body passenger and cargo aircraft, connecting flows | Multiple terminals, jetways, rail/tram, customs/immigration, complex ATC and resilience |

Objectives should teach and test meaningful operation: complete a safe number of movements, sustain a satisfaction threshold, support three compatible tenants, recover from weather disruption, or move a passenger target without excessive delay. Avoid objectives that reward arbitrary placement or waiting.

## Airport systems at concept level

### Construction and land

Runways, taxiways, roadways, parking areas, and rail use grid-assisted drawing tools. Runway heading determines its numbers and markings. Length, width, surface, lighting, and navigation aids determine capability. Finalized projects reserve money and materials; workers and vehicles then build them visibly. Land parcels create strategic expansion choices rather than an unlimited canvas.

### Flight and airside operations

Traffic demand responds to airport capability, rating, contracts, region, time, and weather. Aircraft select compatible runways, routes, stands, and services. Standard patterns govern uncontrolled operations; an unlockable ATC tower increases safe throughput and adds procedural control. PAPI/VASI, runway lights, and instrument procedures progressively extend operating conditions.

ATC and pilot exchanges may later use procedurally assembled text and text-to-speech with a radio treatment. This is an immersion and feedback layer, not a requirement for the first playable version.

### Aircraft turnaround and parking

Stand size and equipment constrain aircraft compatibility. Turnaround tasks can include passenger exchange, fueling, baggage, cargo, cleaning, catering, deicing, maintenance, and pushback. The player should be able to inspect task progress and the reason for every delay. Hangars and long-stay parking serve GA, schools, charter firms, and based operators differently from airline gates.

### Passenger and terminal flow

Passengers move through parking, drop-off, check-in, security, waiting, boarding, and baggage flows appropriate to the airport stage. Each passenger has a compact identity, itinerary, time pressure, and satisfaction state. Terminal detail should support decisions and stories without letting interior decoration eclipse the airfield simulation.

### Ground access

Cars, taxis, rideshare, buses, rental cars, parking lots, and later trains connect passengers to the airport. Rental companies pay for terminal presence and require assigned vehicle capacity. Rail/tram tracks must form valid connections to terminals; the interface should explain broken connections and capacity constraints.

### Tenants and commercial relationships

Airlines rent gates and create scheduled demand. Cargo carriers require compatible cargo stands or hangars. Charter operators and flight schools need distinct facilities. Rental agencies, shops, vendors, and event operators create recurring or event-based income in exchange for space and service quality. Contracts expose requirements and benefits before the player signs.

### Staffing and emergency readiness

Construction, operations, security, ground service, rescue, maintenance, and administrative roles have distinct costs and coverage. Emergency services are primarily a readiness, response-time, rating, and capacity system; the concept does not depend on graphic disasters. Staffing should produce scheduling and coverage decisions rather than repetitive manual assignment.

### Weather, time, and seasons

Day/night, wind, precipitation, fog, snow, temperature, and seasons affect demand and operations. The simulation produces readable METAR-like weather while also translating it into plain-language consequences. Deicing, snow removal, lighting, instrument capability, and runway choice create preparation and recovery gameplay.

### Rating and satisfaction

The overall airport rating is a transparent roll-up of passenger, airline, cargo, tenant, and operational dimensions. It should show the major drivers and recent changes. Higher ratings support stronger demand, better contracts, and pricing power, but aggressive pricing or over-capacity operations can reduce satisfaction.

### Airshows and special events

Airshows are optional, planned events with upfront cost, temporary infrastructure, vendors, traffic, and safety requirements. Visiting aircraft perform short, readable routines before departing. The airport earns from attendance and concessions if it has enough capacity and preparation.

## Economy principles

Revenue can come from landing and parking fees, gate and tenant rent, passenger charges, fuel and service margins, concessions, ground transport, cargo handling, and events. Costs include construction, land, payroll, maintenance, utilities, inventory, vehicles, debt, and closures.

The economy should make growth tempting but not automatic:

- a larger runway increases opportunity and maintenance exposure;
- a new airline brings rent and passengers but creates peak capacity pressure;
- additional emergency capability costs money but permits more demanding operations;
- closing a runway for improvement sacrifices current revenue for future capacity;
- passenger-facing improvements matter because satisfaction affects spending and demand.

All prices and timings from `ideas.txt` remain illustrative until a dedicated economy specification defines scale, cadence, difficulty, and failure recovery.

## Overall gameplay experience

The desired tone is optimistic, tactile, and absorbing rather than punitive. Early sessions emphasize the satisfaction of watching a hand-built airfield begin to work. Mid-game introduces tradeoffs between specialization and broad growth. Late-game becomes an exercise in orchestrating a busy, resilient network without losing the ability to inspect one aircraft or passenger and understand their story.

The player should regularly experience:

- anticipation as a newly unlocked aircraft approaches;
- pride when a layout works without intervention;
- a clear “aha” moment when diagnosing a bottleneck;
- meaningful pressure when weather or traffic challenges capacity;
- visible payoff when construction changes the airport;
- calm periods where watching the airport is enjoyable by itself.

Difficulty should come from interacting systems and imperfect capacity, not hidden rules or arbitrary failures. Pausing, speed controls, overlays, plain-language explanations, scalable UI, color-independent status cues, and forgiving construction planning are foundational accessibility expectations.

## Visual and audio direction

The game uses a 2D cartoon style with a primarily top-down camera. Aircraft silhouettes, runway markings, taxi paths, vehicles, workers, and status states must remain legible at management zoom. Day/night and weather should change atmosphere without hiding operational information.

The existing `AirplaneGame` library provides 46 aircraft sprite files. The 23 catalogued civilian/commercial models cover the entire proposed progression. They are strong prototype and reuse candidates, but their varied pixel dimensions, orientation, scale, outlines, liveries, and one SVG/PNG format difference require an Unreal import and normalization plan. Military and uncatalogued aircraft are out of the core scope unless later justified for airshows, government traffic, or special events. See `../90-research/aircraft-asset-inventory.csv`.

ATC and pilot audio should favor short, comprehensible exchanges. Procedural text-to-speech, radio filtering, and distance/static effects are a later feature that must preserve captioning and allow players to reduce repetition.

## Unreal Engine direction

Unreal Engine is the target runtime and editor. The planning baseline is 2D, even if Unreal represents sprites on planes or uses 3D-capable systems internally. No 3D asset production, 3D aircraft models, or perspective-dependent game design is planned.

Unreal's MCP server will be used after installation for bounded editor inspection, asset validation, project-state checks, and repeatable assistance. MCP availability must not become a runtime dependency of the shipped game. A later technical plan will choose the exact Unreal version, Paper 2D or equivalent rendering approach, grid/path architecture, data assets, save model, performance budgets, and MCP configuration.

## Comparable-game research

The game should learn from the genre without becoming a feature-by-feature clone.

| Reference | Relevant strength | Concept takeaway |
| --- | --- | --- |
| [SimAirport](https://www.simairport.com/) | Frames play as build, optimize, and expand around an agent-based airport with visible passenger and infrastructure flows | Preserve a simple repeated macro-loop and make efficiency problems observable |
| [Airport CEO](https://www.airportceo.com/) | Connects construction, staffing, contracts, passenger needs, and live flight operations | Let business choices produce physical consequences in the airport |
| [Sky Haven](https://store.steampowered.com/app/674090/Sky_Haven/) | Links airport layout, detailed services, land growth, aircraft progression, and multiple operating strategies | Support specialization, but gate complexity so the early airfield remains understandable |
| [Cities: Skylines – Airports](https://www.paradoxinteractive.com/games/cities-skylines/add-ons/cities-skylines-airports) | Uses a clear “budding airfield to global hub” fantasy and integrates public transport, tourism, industry, gates, and levels | Make airport tiers visually transformative and connect the airport to its surrounding economy |
| [Airport Manager](https://store.steampowered.com/app/3248900/Airport_Manager/) | Uses the same grass-strip-to-world-class-hub arc with landing, fuel, baggage, service fees, and facility upgrades | This arc is intuitive but not unique; differentiation must come from top-down systemic depth, construction crews, real-aircraft behavior, and readable aviation rules |
| [Airport Master](https://store.steampowered.com/app/584280/Airport_Master/) | Pursues real aircraft, IFR/VFR procedures, ATC, weather, staff, fuel contracts, and regional-airport operations | Aviation depth is attractive, but the design must stage it carefully and protect playability from unchecked scope |

The local screenshots reinforce six useful presentation ideas:

- show the whole starter airfield as a coherent footprint;
- make specialized services such as deicing visible at aircraft scale;
- keep night operations readable;
- represent GA stands and fuel infrastructure distinctly;
- expose turnaround tasks and connectivity in an inspectable overlay;
- give each aircraft a flight identity and itinerary.

These are research prompts, not requirements to reproduce another game's interface.

## Scope boundaries

### In the core concept

- Single-player airport construction and management
- 2D top-down cartoon presentation
- Grass airfield through international-hub progression
- Real-world civilian aircraft types where legally and practically suitable
- Visible aircraft, passenger, staff, service, and ground-access flows
- Weather, runway selection, operational capability, objectives, economy, and rating
- Unreal Engine and an MCP-assisted development workflow

### Not assumed for the first playable version

- Full terminal-interior simulation
- Full ATC workplace simulation
- Procedural voice generation
- International operations, wide-bodies, trains, airshows, and every tenant type
- Detailed emergencies or aircraft accidents
- Multiplayer
- Modding

### Explicitly out of scope

- 3D presentation or 3D asset use
- Reusing the `AirplaneGame` 3D models
- Combat as a core system
- Building code before the planning baseline is approved

## Suggested first playable experience

The smallest version that proves the identity is one compact map, one road entrance, visible delivery and construction workers, a placeable grass runway/taxiway/stand, an airport open/close control, a handful of reusable GA aircraft, wind-based runway direction, simple arrival/parking/departure behavior, landing and parking revenue, one upgrade objective, and entity inspection.

This is not yet an implementation commitment. It is a concept test: can the player build a tiny airfield, understand why aircraft behave as they do, enjoy watching it operate, and feel motivated to make the next meaningful upgrade?

## Open concept questions

- What working title best expresses the game's identity?
- Is the map a fictional region, a choice of biome/region, or a real-world-inspired location?
- How long should progression from first grass strip to regional service take?
- How much terminal interior detail belongs in the primary camera and simulation?
- Should failure allow debt, bailout, restructuring, or only restart/recovery objectives?
- Are real manufacturer/model names and recognizable liveries acceptable after rights review, or should aircraft use fictionalized brands with realistic performance classes?
- Which specializations should be viable through the late game rather than merely stepping stones?
- How directly should the player control ATC versus setting rules and staffing an automated service?
- What audience balance is desired between aviation enthusiasts, management-sim players, and younger/casual players?

## Approval criteria for the next planning stage

Move this document to `Approved` only when the team agrees on:

- the concept statement and five design pillars;
- 2D Unreal direction and no-3D boundary;
- the local-to-international progression structure;
- the balance between aviation authenticity and approachable management;
- the intended player audience and difficulty posture;
- the first-playable identity test;
- whether real aircraft branding is a product requirement or an optional content direction.

