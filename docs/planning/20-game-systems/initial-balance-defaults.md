# GS-20: Initial Balance Defaults

**Status:** Approved baseline  
**Owner:** Balance  
**Last updated:** 2026-07-25

## Purpose

This document supplies coherent initial tunable values so implementation and tests are not blocked. Values are data, not hard-coded constants, and may change through measured playtesting without redefining system behavior.

## Core time and scale

| Value | Initial default |
| --- | ---: |
| Game time at 1x | 1 game minute per real second |
| Speeds | Pause, 1x, 2x, 4x, 8x |
| Timetable increment | 5 game minutes |
| Locked operations horizon | 30 game minutes |
| Suggested objectives | Maximum 3 |
| Starter offer after readiness | Immediate while paused |
| First guided flight target | Within 3 game minutes after schedule confirmation |
| Logical-agent target | 10,000 |
| Simultaneously visible agent target | 2,000 |
| On-map aircraft target | 150 |
| On-map service/landside vehicle target | 500 |

## Starting package

The default save begins with 5,000 Credits, 0 Airport Points, the starting parcel, external road portal, construction access, and the first capability group unlocked. A valid basic grass-airfield plan—small runway, taxi connection, one GA stand, windsock/markings, and essential access—must cost no more than 3,500 Credits, leaving at least 30% contingency.

Planning fee is 2% of project price. Cancellation before delivery returns 98%. Delivered materials retain 60% of their unspent value; completed work is not refunded. Demolition salvage is 10% of the demolished facility's current base cost.

## Economy formulas

Construction price:

`Base definition cost x regional factor x size factor + site preparation + connection cost`

Operating expense per accounting hour:

`team wages + facility operating cost + vehicle operating cost + financing cost`

Contract payout:

`base reward x completion factor x controllable punctuality factor x service factor + safe optional-service bonuses`

The default accounting interval is 60 game minutes. Ordinary controllable delay begins after 5 game minutes; the penalty reaches its contract cap at 60 minutes. External delay does not reduce airport punctuality but remains visible.

Recovery grant restores liquid Credits to the lesser of 2,000 or the amount needed for a valid basic airfield plan. Rescue loans are capped so scheduled repayment never exceeds 20% of the trailing average revenue; interest does not compound while revenue is zero.

## Rating and progression

Overall rating weights are safety 30%, reliability 25%, customer experience 20%, tenant relationships 15%, and access/cleanliness/amenities 10%. Inapplicable components are removed and remaining weights normalized.

Capability bands require both Airport Points and path evidence:

| Band | Airport Points | Evidence window |
| --- | ---: | ---: |
| Established | 5 | Demonstrate one complete operational loop |
| Regional | 20 | 3 operating days |
| Advanced | 50 | 7 operating days |
| Major | 100 | 14 operating days |

The target for a first engaged save to reach one Major specialization is 10–15 real hours. Airport Points are awarded mainly for first-time capabilities and achievements, limiting repetitive grinding.

## Capacity and service defaults

Simulation data defines real task quantities and type-relative performance. Initial schedule planning reserves a 5-game-minute arrival and departure stand buffer. Queue warnings begin at 70% sustained capacity and become high risk at 90%. A service or passenger entity reports blocked after 5 game minutes without progress.

Near-term weather forecast covers 6 game hours with high confidence; trend forecast extends to 24 game hours. Ordinary offers remain available for 2 game days; pinned offers remain for 4.

## Balance validation

Before a phase closes, automated headless scenarios must verify solvency and progression for GA, flight school, charter, cargo, passenger, and mixed paths. Playtests then measure first-flight time, explanation comprehension, idle time, recovery use, Major-capability time, and dominant strategies. Any change that violates recoverability or path viability requires specification review, not a silent tuning edit.
