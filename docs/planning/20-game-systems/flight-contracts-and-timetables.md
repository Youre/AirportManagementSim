# GS-04: Flight Demand, Contracts, and Timetables

**Status:** Approved baseline  
**Owner:** Commercial planning  
**Last updated:** 2026-07-25

## Outcome and scope

Operators offer understandable opportunities; the player decides which flights to accept, their exact timetable slots, and their gates or stands. This specification owns demand, offers, contracts, scheduling, feasibility, recurring services, and disruption choices. A worldwide route network is intentionally background fiction.

## Offers and contracts

Offer generation considers airport capability, specialization, rating, region, time of day, season, operator relationship, and recent performance. Offers identify aircraft type, role, origin/destination region, requested frequency, acceptable arrival window, service requirements, estimated passengers or cargo, revenue, penalties, and contract length.

Offer classes are:

- one-time visit;
- recurring general-aviation, charter, cargo, or passenger movement;
- flight-school activity block;
- tenant-linked service;
- authored special-event movement.

The player may accept, decline, or pin an offer. Declining has no punishment. Pinned offers expire later than ordinary offers. Acceptance creates a contract but does not place flights until the player chooses exact slots and compatible stands.

## Timetable

The timetable uses local airport time and a seven-day repeating planning view plus a dated operations view. Each flight has arrival, planned stand time, departure, runway preference if allowed, and assigned stand/gate. The player may drag or enter exact times in five-game-minute increments.

Feasibility validation evaluates runway throughput, taxi conflicts at a coarse planning level, stand occupancy including buffers, required services, terminal and baggage capacity, curfew or weather constraints, and aircraft compatibility. Warnings are classified:

- `Blocked`: physically or contractually impossible;
- `High risk`: likely failure under normal variation;
- `Advisory`: viable but with limited resilience.

The player may override high-risk warnings but not blocked rules. Overrides are recorded for incident traceability.

## Day of operation

At the operational horizon, a scheduled flight becomes a persistent flight instance tied to an airframe. Routine ATC and service dispatch are automatic. The player can change priority, gate, runway preference, delay, cancel, or request diversion when rules allow. Gate changes require a valid passenger, baggage, and service route.

Early arrivals wait or use an available compatible stand if policy permits. Late flights keep their identity and propagate conflicts. Recovery choices show predicted passenger, tenant, rating, and financial effects before confirmation.

## Performance and renewal

Contracts track completion, controllable delay, cancellations, service quality, safety, and tenant satisfaction. Renewal offers reflect performance without creating an irreversible failure spiral. Poor performance may reduce frequency or rewards; a basic recovery offer pool always remains available.

## Visual reference

Use [VA-04 Flight planning and timetable](../30-content-and-assets/concept-art/04-flight-planning-timetable.png) to validate the contract-to-slot workflow, exact gate ownership, flight-card scanability, localized conflict treatment, and adjacent feasibility evidence. The seven-day data model and GS-20 values control the real schedule; depicted times, rewards, route codes, and density are non-normative.

## Defaults and acceptance

The planning horizon is seven days; the locked operational horizon is 30 game minutes. Changes inside the locked horizon issue a high-risk warning but remain possible unless an aircraft has begun the affected movement. Automated tests must prove exact-slot ownership, stand exclusivity, recurring-instance creation, late-flight conflict propagation, override traceability, recovery offers, and stable results after save/load.
