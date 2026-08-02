# GS-02: Networks, Connectivity, and Facility Validation

**Status:** Approved baseline  
**Owner:** Networks and validation  
**Last updated:** 2026-07-25

## Outcome and scope

Players draw readable networks and understand why a facility does or does not work. This specification owns network topology, connection ports, zones, validation, and diagnostic overlays. Routing algorithms are defined by TS-03.

## Network families

Authoritative networks are separate graphs:

- aircraft movement: runways, taxiways, taxi lanes, holding points, stands, and gates;
- airside service: service roads, depots, staging areas, and restricted crossings;
- public transport: external roads, airport roads, stops, parking, rental-car access, and rail;
- pedestrian: walkable terminal areas, doors, stairs or ramps, queues, and controlled boundaries;
- baggage: check-in feeds, conveyors, screening, make-up, transfer, and reclaim.

Utilities are abstract. A building needs an enabled utility connection flag derived from construction and ownership; the player does not lay pipes, power lines, or data cable. Infrastructure degradation and routine wear are out of scope.

## Graph and port rules

- Drawn networks snap to the hierarchical grid while presenting smooth, readable paths.
- Each facility definition declares typed ports, capacity, direction, accessibility, security side, and compatible network.
- Connections occur only between compatible ports within tolerance and with a traversable link.
- Network validity is evaluated over the complete connected component. A
  facility-to-runway path may traverse any number of compatible intermediate
  segments; a valid path does not excuse a separate orphan segment in the same
  proposal.
- Crossings are explicit objects. A road may not silently cross a taxiway, secure boundary, belt, or pedestrian route.
- Closing a node or segment updates reachability and reservations at the next simulation boundary.
- Zones group cells and facilities for staff coverage, security, terminal rooms, construction, and emergency response. Overlap is allowed only between compatible zone types.

## Facility validity

A facility is operational only when it is built, opened, not incident-closed, and all required rules pass. Rules may require:

- owned and buildable footprint;
- minimum dimensions and safety clearance;
- connection to one or more named network ports;
- path to an external source, destination, depot, or controlled boundary;
- compatible upstream and downstream capacity;
- assigned staff team or tenant;
- no forbidden zone crossing;
- required supporting equipment.

Validation results use stable reason codes, severity, affected entity IDs, and a player-facing remedy. A facility can be `Valid`, `Degraded`, `Blocked`, or `Closed`. Degraded means operation is allowed with a disclosed capacity or rating effect.

## Diagnostics

Selecting an invalid facility highlights the failed port, broken path, or conflicting zone and offers the relevant overlay. Network overlays show direction, reachability, congestion, capacity, reservations, and closed segments. Diagnostics must use authentic terms followed by concise explanations; for example, "No sterile route: departing passengers can reach the gate without passing security."

## Change propagation

Network edits are evaluated as proposals before commitment. Committed construction changes activate only when inspected. When a link closes, dependent systems receive one topology-changed event and recalculate lazily by affected component; they do not poll the entire map. Existing travelers may finish a safe segment before rerouting, but they never traverse a newly forbidden security boundary or active work zone.

## Acceptance

Automated graph fixtures must prove directionality, crossings, disconnected components, security boundaries, capacity degradation, closure propagation, and restoration after reopening. Every blocked facility in the fixtures must produce one primary reason and an actionable remedy. Identical topology and commands must yield identical validation results after save/load.
