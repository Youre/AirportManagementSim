# GS-11: Staff Teams, Zones, and Coverage

**Status:** Approved baseline  
**Owner:** Workforce  
**Last updated:** 2026-08-03

## Outcome and scope

Players manage operational capability through understandable role-based teams and coverage zones, not individual shift micromanagement. This specification owns roles, teams, qualifications, zones, staffing level, dispatch availability, workload, and morale.

## Roles and teams

Roles include construction, ramp, baggage, fueling, cleaning/catering, maintenance response, security, border/customs, customer service, emergency/fire, and airport police. Content may add specialized roles with the same contract.

The player hires a team with a role, size, home facility, qualification tier, wage, operating schedule, and one or more allowed zones. Individual people may be named visually, but the authoritative labor unit is the team. Individual recruitment, payroll negotiation, careers, and detailed rosters are out of scope.

## Coverage and scheduling

Teams are available when on schedule, present at a valid home facility, able to reach the task, qualified, and below simultaneous-capacity limits. The default schedule covers airport operating hours; players can add early, late, or continuous coverage blocks.

Zones are optional restrictions used to shorten response and reserve capacity. An unzoned team serves all reachable compatible tasks. Overlapping zones use priority and workload. Emergency teams may cross normal zones under emergency policy.

## Workload and morale

Workload is the ratio of assigned effort to available effort over a rolling window. Sustained overload slows service and lowers team morale; safe staffing and break facilities restore it. Morale is a compact team value, not an individual psychological simulation. Teams do not quit without warning. A predicted shortage appears before the schedule period begins.

## Assignment

Routine tasks dispatch automatically through GS-07 and GS-18. Players set role priorities, flight priorities, zone priority, and reserve capacity. A manual reassignment is allowed if it does not abandon an active safety-critical task.

## Phase 1 staff surface

Once the first airport identity exists, the compact map rail exposes a Staff
action. Its Phase 1 panel is read-only and reports the four-person starter
construction crew, available and assigned counts, current construction status,
and automatic-dispatch behavior. Before funding, all four are available. From
funding through inspection, all four are assigned; after completion they return
to availability. The panel is closed by default, toggles from the Staff icon,
and closes when build mode takes over the map.

This early surface does not introduce individual rosters, hiring, manual work
orders, or shift controls. Later workforce screens extend the same authoritative
team records rather than replacing this summary with a separate counter.

## Acceptance

Tests must prove qualification, schedule, reachability, zone priority, overlapping zones, overload, recovery, emergency override, and save/load. Removing or closing the last qualified team for an active safety requirement must warn and identify affected facilities or flights.
