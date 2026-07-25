# TS-03: Spatial Grid, Routing, and Reservations

**Status:** Approved baseline  
**Owner:** Spatial systems  
**Last updated:** 2026-07-25

## Outcome

One coherent spatial model supports construction, terminal rooms, pedestrians, vehicles, baggage, aircraft clearance, connectivity, and diagnostics at airport scale.

## Coordinate model

Simulation coordinates use integer centimeters, with a canonical one-meter planning cell. Outdoor network placement snaps to five-meter control points by default while curves occupy precise centimeter geometry. The world is divided into 64 x 64 meter chunks. Maps may extend to 8 x 8 kilometers, but only purchased/active chunks allocate detailed occupancy.

Floors use integer level IDs and authored elevation offsets. World origin and map orientation are stable save data. Render transforms are derived.

## Spatial layers

Each chunk maintains versioned layers for land ownership, terrain/buildability, structure occupancy, walkability, security zone, movement pavement, service road, public road, baggage, work zone, safety clearance, and temporary closure. Changes mark affected chunks and connected components dirty.

Broad-phase queries use chunk indices and bounding boxes. Exact facility/aircraft clearance uses integer polygons or swept envelopes. Visual sprite bounds are never collision authority.

## Routing

- Pedestrians: hierarchical A* over walkable cells, portals, queues, and vertical connectors.
- Roads/taxiways/baggage: graph search over typed directed nodes and edges with capacity, closure, and compatibility costs.
- Aircraft: route search additionally checks turning/wing clearance and rolling block reservation.
- Emergency response: priority graph with restricted-link permission and response-time estimate.

Routes store graph revision. On a relevant topology change, an entity finishes its safe current segment, then validates/reroutes. Path caches key on endpoints, entity capability, policy, and graph revision and have bounded memory.

## Reservations and congestion

Reservable resources include taxi/runway blocks, intersections, service crossings, queue positions, stand footprints, belt buffers, doors, and equipment ports. Requests have owner, interval, priority, direction, capacity units, and expiry. The resolver uses stable priority then request sequence for deterministic ties.

Deadlock detection builds a bounded wait-for graph. Resolution cancels the lowest safe-priority future reservation or reroutes; it never moves an entity through a conflict. Persistent topology deadlock creates a player diagnostic.

## Acceptance

Fixtures must cover multi-floor passenger routes, security boundaries, outdoor-to-indoor paths, large aircraft clearance, directed roads/belts, chunk activation, closures, reservation conflicts, deterministic ties, cache invalidation, and deadlock recovery. Performance tests use the maximum map and target populations from GS-20.

