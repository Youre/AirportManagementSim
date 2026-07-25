# GS-09: Baggage System

**Status:** Approved baseline  
**Owner:** Baggage  
**Last updated:** 2026-07-25

## Outcome and scope

Checked bags visibly travel through a system the player designs, making baggage performance understandable rather than abstract. This specification owns bag identity, conveyors, screening integration, make-up, carts, loading, transfers, reclaim, reconciliation, and exceptions.

## Bag lifecycle

Each bag has a stable ID, passenger or cargo owner, flight, origin process, destination process, screening state, route, container/cart assignment, deadline, and exception state. Departing bags move through `Accepted -> Conveyor -> Screened -> Sorted -> Make-up -> Cart/container -> Aircraft`. Arrivals move through `Aircraft -> Cart/container -> Infeed -> Reclaim -> Collected`. Transfers join the appropriate outbound sort path.

## Conveyor network

Players place directional straight, corner, merge, split, lift, scanner, manual-search, make-up, infeed, and reclaim components. Belts visibly carry representative bag sprites. Each component declares throughput, buffer, directions, floor connection, and allowed security transition.

Routing chooses a valid destination while respecting direction, screening, capacity, and flight cutoff. Merges use fair reservation; full buffers propagate backpressure. A belt cannot pass through a wall, unsupported floor, or uncontrolled security boundary.

## Ramp handling and reconciliation

Make-up positions group bags by flight. Baggage teams load carts or compatible unit-load equipment and drive via service roads. Aircraft loading requires the correct flight, service access, staff, and capacity. Before departure, reconciliation reports accepted, loaded, offloaded, missing, and passenger-withdrawal bags.

A flight may depart with a mishandled bag only under explicit policy and legal/safety rules. A boarded passenger's known checked bag cannot be silently discarded from state.

## Exceptions and feedback

Unscreened, jammed, misrouted, late, unidentified, or unclaimed bags enter visible exception handling. No random degradation creates jams; overload, invalid topology, closure, incident, or authored event must explain the cause. The overlay shows direction, load, buffer fill, route, cutoff risk, and security status.

## Visual reference

[VA-03 Terminal and passenger flow](../30-content-and-assets/concept-art/03-terminal-passenger-flow.png) is the reference for making conveyors, make-up, reclaim, bags, and passenger ownership legible within the terminal rather than hiding baggage behind a summary statistic.

## Acceptance

Tests must prove directionality, merge fairness, backpressure, screening enforcement, flight sorting, transfer routing, cart capacity, reconciliation, passenger offload, reclaim collection, exception recovery, and save/load mid-route. Logical bag totals must reconcile at every simulation step.
