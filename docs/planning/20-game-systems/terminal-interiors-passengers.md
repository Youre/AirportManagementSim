# GS-08: Terminal Interiors and Passenger Flow

**Status:** Approved baseline  
**Owner:** Terminal and passengers  
**Last updated:** 2026-07-25

## Outcome and scope

Players design complete terminal interiors and watch named passengers navigate a readable journey. This specification owns floors, rooms, doors, pedestrian flow, passenger parties, needs, itinerary states, boarding, connections, and reclaim. Security processing is owned by GS-10 and bags by GS-09.

## Interior construction

Terminals use the shared one-meter grid. Players draw foundations, floors, walls, doors, windows, controlled doors, stairs/ramps where required, queues, seating, signs, counters, concessions, restrooms, and decorative functional objects. Every walkable cell belongs to a floor and may belong to a room and security zone.

Rooms are inferred from enclosed walkable space and annotated by function. A room can remain mixed-use unless a facility requires a dedicated type. Upper floors are supported for terminals; aircraft movement remains on the ground plane. Elevators are abstracted with accessible vertical-connection nodes rather than simulated cabins.

## Passenger entities

Passengers have stable names, age band, travel party, itinerary, mobility profile, arrival mode, bag count, patience, and a compact need set: time confidence, comfort, food/drink, restroom, and information. Needs influence choices and satisfaction but are not survival mechanics.

Travel parties stay together unless capacity or a controlled process temporarily splits them. Children are never depicted unattended through distress mechanics. Names are fictional and generated locally.

## Journey

Departing states are `Approaching airport -> Landside entry -> Check-in/bag drop if needed -> Security -> Border exit if required -> Gate area -> Boarding -> Aircraft`. Arriving states reverse through border entry where required, baggage reclaim, customs where required, and ground transport. Connecting passengers follow a route determined by domestic/international and security status.

Passengers choose valid paths and optional services based on remaining time, needs, queue forecasts, walking distance, price, and preference. They do not have omniscient knowledge; signs and information facilities improve decisions. When no path exists they wait, seek information, and generate an actionable alert rather than disappearing.

## Capacity and boarding

Queues use physical waiting capacity. Counters, doors, seating, buses, and boarding methods have throughput and eligibility. Boarding reconciles passenger identity and flight; late passengers may be paged and eventually offloaded under policy. Accessibility profiles require accessible paths and reasonable service priority.

## Scale

Every logical passenger remains named and inspectable. At high population, decision updates are staggered and visuals are pooled or aggregated by TS-02/TS-06. Aggregation may reduce animation density but cannot change counts, queues, itineraries, or outcomes.

## Visual reference

Use [VA-03 Terminal and passenger flow](../30-content-and-assets/concept-art/03-terminal-passenger-flow.png) to validate a readable full-terminal cutaway, journey direction, party inspection, compact needs, congestion, secure boundaries, accessible paths, baggage visibility, and curb connection. Exact room layout and population are illustrative.

## Acceptance

Tests must prove room detection, door/security boundaries, complete departing/arriving/connecting journeys, party cohesion, accessible routing, queue capacity, missed-flight handling, path-failure diagnostics, and save/load identity. A 10,000-passenger logical fixture must preserve totals and determinism.
