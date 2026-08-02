# GS-06: Taxi, Ground Movement, Stands, and Gates

**Status:** Approved baseline  
**Owner:** Surface movement  
**Last updated:** 2026-07-25

## Outcome and scope

Aircraft visibly and safely travel between runways and compatible parking positions. This specification owns taxi routing, movement reservations, holding points, runway crossings, stands, gates, towing, and surface congestion.

## Movement network

Taxiways and apron taxilanes are directional graph segments with centerline, width class, pavement class, speed, clearance envelope, and optional name. Holding points and crossings divide routes into reservable blocks. Routes reject inadequate width, surface, turning radius, wingtip clearance, closed links, and incompatible crossings.

For the starter-airfield construction journey, the curated map provides a
small operations terminal shell with two aircraft-side gate/stand ports. The
player draws the runway and one or more taxiway segments. The starter network
is usable when one connected component reaches a runway and at least one of
those ports; taxiway-to-taxiway joins and legal runway intersections count as
ordinary graph connections.

The automatic dispatcher chooses a safe route minimizing travel time, conflicts, and runway crossings. The player may set one-way directions, close segments, set route preferences, assign a different stand, or request a tow. Direct waypoint driving is out of scope.

## Reservations and movement

Aircraft request a rolling reservation window instead of reserving an entire route. Conflicting blocks, runway authority, service vehicles, and work zones can cause a hold. Deadlock detection chooses a low-cost aircraft to release or reroute and generates a diagnostic if player topology prevents resolution.

Surface states are `Awaiting clearance`, `Taxiing`, `Holding`, `Crossing runway`, `Entering stand`, `Parked`, `Pushback`, `Towed`, and `Blocked`. Kinematic movement follows aircraft-type turn and speed limits. Presentation interpolation cannot enter an unreserved block.

## Stands and gates

A stand definition specifies aircraft size, role, orientation, entry/exit mode, pushback requirement, boarding mode, service ports, secure-side relationship, and optional gate/terminal connection. Compatibility is evaluated before assignment.

A stand reservation includes arrival buffer, occupancy, turnaround, departure buffer, and contingency release. Two aircraft cannot occupy or reserve conflicting footprints. Remote stands may use buses; contact gates require a valid sterile passenger route. GA tie-downs, hangars, cargo stands, and flight-school parking remain first-class endgame choices.

## Crossings and vehicles

Runway crossings require ATC authority. Aircraft have priority on movement areas except where emergency policy applies. Service-road crossings use signals or controlled reservations and cannot create invisible clipping. An aircraft blocked beyond the configured threshold reports the exact blocking entity or topology issue.

## Acceptance

Tests must prove size and pavement compatibility, rolling reservations, head-on avoidance, runway-crossing authority, pushback clearance, stand exclusivity, towing, deadlock recovery, closure rerouting, and deterministic post-load continuation. A dense fixture must show no overlapping aircraft footprints and no unbounded route recalculation.
