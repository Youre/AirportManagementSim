# GS-07: Turnaround and Ground Services

**Status:** Approved baseline  
**Owner:** Ground services  
**Last updated:** 2026-07-25

## Outcome and scope

Parked aircraft receive visible, comprehensible services through automated teams and vehicles while the player manages capacity, policies, and exceptions. This specification owns turnaround task graphs, service requests, dispatch, equipment, priorities, and readiness.

## Task model

An aircraft turnaround is a dependency graph assembled from aircraft type, flight role, load, operator contract, airport policy, and current condition. Tasks can include deboarding, boarding, fueling or charging where applicable, baggage unload/load, cargo handling, catering, cabin cleaning, potable water, lavatory service, inspection, deicing, pushback, and optional maintenance response.

Tasks declare prerequisites, exclusion zones, staff roles, vehicle/equipment types, service ports, quantity, duration function, and completion effect. Compatible tasks may run in parallel; unsafe combinations such as fueling conflicts are blocked by explicit rules. There is no universal fixed turnaround duration.

## Automation and control

Routine requests dispatch automatically using player policies. The player controls team zones, fleet capacity, target service level, task priority, optional services, and flight priority. Individual overrides can cancel, defer, expedite, or reassign a task when safe. Manual vehicle driving is out of scope.

Dispatch scores compatible resources by urgency, travel time, zone, current queue, remaining capacity, and repositioning cost. A task never completes without the required team, equipment, route, and service access.

## Readiness and delay

Departure readiness requires all mandatory tasks, passenger/bag/cargo reconciliation, closed doors, released safety zones, paperwork state, and departure clearance eligibility. Optional-service omission affects contract or satisfaction results but may not block departure.

The turnaround panel shows a dependency timeline, active resource, predicted completion, blocker, and controllable delay. Delay attribution separates late arrival, airport capacity, operator cause, weather, ATC, passenger, security, and incident causes.

## Vehicles and depots

Vehicles are persistent entities with type, capacity, assignment, depot, load, route, and state. They refill or unload at compatible depots. They do not consume simulated fuel or degrade routinely. Collisions occur only through traceable serious-risk scenarios, never random background wear.

## Visual reference

[VA-05 Aircraft turnaround](../30-content-and-assets/concept-art/05-aircraft-turnaround.png) is the close-operations reference for aircraft-centered composition, safe vehicle choreography, automatic dispatch, concurrent versus waiting tasks, dependency timeline, priority controls, and predicted readiness. Service compatibility, positions, and durations must come from simulation data rather than the illustration.

## Acceptance

Tests must prove dependency ordering, legal parallelism, incompatible-operation exclusion, quantity-based duration, automatic dispatch, priority override, depot refill, route blockage, departure-readiness gating, delay attribution, and save/load mid-task. The target load must run with pooled visuals and no authoritative per-vehicle Actor Tick.
