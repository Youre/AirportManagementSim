# GS-13: Ground Access and Transport

**Status:** Approved baseline  
**Owner:** Landside transport  
**Last updated:** 2026-07-25

## Outcome and scope

Passengers, staff, workers, and deliveries physically reach airport destinations through functional landside networks. This specification owns roads, curbs, parking, taxis, buses, rental cars, rail, external demand, and mode choice.

## Network and external connection

Every map has at least one external road portal. Road segments define direction, lanes abstracted as capacity, speed, vehicle permissions, stops, intersections, and curb access. The map may expose a rail connection site; rail becomes usable only after the player builds a station and funds service.

Vehicle classes are private car, taxi, rental-car shuttle/vehicle, public bus, staff shuttle, delivery truck, construction vehicle, and emergency vehicle. Traffic is kinematic and route-based. Detailed lane changing, parking maneuvers, fuel consumption, and regional traffic simulation are out of scope.

## Passenger mode choice

Passenger parties choose a mode from those available using cost, journey time, wait, walking distance, party profile, and service hours. The game generates external arrivals early enough for a reasonable journey, while congestion and undersupply can still cause late passengers.

Parking requires an open entrance, reachable spaces, and pedestrian path. Taxi and rides use curb capacity. Rental cars require a tenant, desk or digital pickup point, shuttle/garage access, and available capacity. Buses require stops and a service frequency. Trains require station access, service frequency, and pedestrian connection.

## Operations

Curbs, intersections, lots, and stops have throughput and queue limits. Vehicles may wait off-map when the entry queue is full, but delay remains counted. Delivery and construction access is separated where possible; crossing the public network is allowed with capacity effects. Emergency vehicles receive safe priority.

The landside overlay shows flow, queues, mode split, travel time, parking use, stop wait, inaccessible destinations, and delivery paths.

## Acceptance

Tests must prove each transport mode end-to-end, capacity queues, service hours, parking occupancy, rental-car dependencies, rail connection, emergency priority, disconnected-route diagnostics, and save/load. Passenger counts entering and leaving the region must reconcile with terminal and aircraft populations.

