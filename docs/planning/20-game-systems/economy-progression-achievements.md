# GS-15: Economy, Rewards, Progression, and Achievements

**Status:** Approved baseline  
**Owner:** Economy and progression  
**Last updated:** 2026-07-25

## Outcome and scope

The economy makes planning meaningful while keeping every airport recoverable. Progression guides discovery without prescribing one endgame. This specification owns currencies, revenue, expenses, capability unlocks, grants, loans, objectives, achievements, and recovery assistance.

## Resources

The game has two resources:

- `Credits`: construction, land, vehicles, facilities, staffing, and operating costs.
- `Airport Points`: earned from first-time capabilities, objectives, achievements, and safe reliable operation; spent only to unlock new capability families.

There is no premium currency, real-money purchase, global account progression, or cross-save transfer. Each save owns its resources and unlocks.

## Financial model

Revenue categories are landing/parking fees, flight and cargo completion, tenant rent or share, passenger services, fuel/service charges, parking/transport, and event rewards. Expense categories are construction, land, team wages, tenant support, vehicle/facility operation, refunds/penalties, incident response, and financing.

At each accounting interval:

`Net cash flow = total recognized revenue - total recognized expense`

Every transaction has a category, source entity, game timestamp, amount, and explanatory text. Forecasts separate committed from projected values and never count an offer as revenue.

## Unlocks and pacing

Unlocks depend on Airport Points plus prerequisite capabilities, not passenger count alone. The intended first-save pacing is:

- a functioning GA airfield quickly;
- multiple specialization choices within the first few hours;
- Regional capability during the middle game;
- Major capability in roughly 10–15 hours of engaged play;
- no final ending or forced prestige reset.

Unlocks reveal a small coherent group at a time. Existing facilities keep working after balance updates or unlock revisions.

## Objectives and achievements

Objectives are optional, contextual, and limited to three suggested items at once. They teach or suggest reachable improvements and may be replaced without penalty. Achievements are permanent within the save and celebrate safe operations, construction, aircraft variety, specialization, service quality, and recovery. Neither system requires all specialization paths.

## Recovery

The airport cannot enter an unrecoverable bankruptcy state. If available credits cannot sustain a basic operation, the game offers progressively stronger tools: operating-cost advice, payment deferral, recovery contracts, a low-cost grant, and a capped rescue loan. The final safety net preserves or supplies one usable airfield loop and essential access. Assistance is recorded but never blocks achievements.

## Visual reference

Use [VA-07 Progression and specializations](../30-content-and-assets/concept-art/07-progression-specializations.png) to validate optional guidance, path freedom, capability-band readability, and aspirational progression without an ending screen. All depicted point totals, requirements, rewards, and objective wording are non-normative.

## Acceptance

Tests must prove transaction reconciliation, forecast separation, independent saves, path-neutral unlocks, objective replacement, recovery escalation, no negative-interest spiral, and continued play after severe loss. Simulations for each specialization must reach Major using approved defaults without mandatory passenger-hub facilities.
