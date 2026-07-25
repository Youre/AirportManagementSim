# GS-03: Aircraft Types and Persistent Airframes

**Status:** Approved baseline  
**Owner:** Aircraft domain  
**Last updated:** 2026-07-25

## Outcome and scope

Aircraft are recognizable, operationally meaningful visitors rather than disposable schedule tokens. This specification owns aircraft-type capabilities, individual airframes, compatibility, persistence, and visit history. Movement is owned by GS-05 and GS-06; content selection is owned by CT-02.

## Type definitions

Real aircraft model names may be used after accuracy and rights review. Operators and liveries are fictional. Each type definition provides:

- display name, manufacturer, category, role, and sprite set;
- length, wingspan, turning class, approach category, runway requirements, and pavement class;
- passenger seats, cargo volume and mass, fuel capacity, and service capabilities;
- compatible stand, gate, boarding, fueling, baggage, cargo, and emergency equipment;
- nominal taxi, approach, turnaround-task, noise, and weather limits;
- required content provenance and accuracy status.

Categories include light piston, turboprop, regional jet, narrow-body, wide-body, business jet, helicopter where supported, and cargo variants. A type may support multiple operational roles without duplicating its physical data.

## Persistent airframes

Every accepted flight references an `FAircraftInstanceId`. An airframe records type, fictional operator, tail number, condition flags relevant to current operations, fuel state, payload summary, home region, visit count, prior delays, incidents, and relationship notes. Repeated visits reuse the same identity when plausible. The player can inspect an aircraft logbook using child-readable text.

Airframes exist logically while away. Off-map simulation is limited to determining eligibility and next offered arrival; the game does not simulate a global route network. Independent saves never share airframes or progression.

## Compatibility

Compatibility is an explicit rule result, not an inferred sprite size. Before a flight is scheduled, the game checks runway length/surface, approach aid and weather minima, taxi width/clearance, stand size, service availability, terminal or cargo capability, and any tenant requirement. Each failure reports the limiting value and remedy.

An aircraft may divert or cancel if changing conditions make arrival unsafe. It may remain parked if departure requirements fail. The system must never teleport an incompatible aircraft through the airport to satisfy a schedule.

## State model

On-map states are `Inbound`, `Approach`, `Landing`, `Runway roll`, `Taxi in`, `Parked`, `Turnaround`, `Ready`, `Taxi out`, `Takeoff`, `Outbound`, plus `Holding`, `Towed`, and `Emergency`. Airframe state transitions occur through domain commands and events. Visual animations never authorize a transition.

## Inspectability and safety

Aircraft panels show authentic type, tail number, operator, flight, origin/destination region, occupants or payload, assigned runway/stand, current clearance, service status, and delay reason. Serious damage is communicated through status and reports, without graphic imagery or visible human harm.

## Acceptance

Tests must prove stable airframe identity across multiple visits and save/load, rejection of every major incompatibility family, role variants using shared type data, no array-index identity, and deterministic state transitions. Content validation must reject duplicate tail numbers within an operator, missing sprite directions, impossible dimensions, and unreviewed real branding.

