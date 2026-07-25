# GS-18: Emergency Readiness, Incidents, and Recovery

**Status:** Approved baseline  
**Owner:** Safety and recovery  
**Last updated:** 2026-07-25

## Outcome and scope

Emergencies test preparation and decision-making without depicting human suffering or destroying a save. This specification owns readiness, risk signals, incident triggers, emergency response, abstract outcomes, investigation, closures, repair, and recovery.

## Readiness

Emergency capabilities include airport fire/rescue, medical response, airport police, operations inspection, emergency access roads, stations, equipment, trained teams, coverage zones, and response policies. The readiness panel reports required category, available category, predicted response time, coverage gaps, and current blockers.

## Traceable risk

Serious incidents cannot arise as arbitrary punishment. Each incident instance records contributing risk factors, warnings shown, player decisions or overrides, current conditions, randomness seed, and mitigations. Factors may include accepting operations beyond disclosed weather margins, inadequate emergency category, excessive scheduling congestion, an unsafe override, blocked access, or an unresolved known hazard.

Before a serious risk can produce aircraft loss, injury, or fatality, the player must receive a clear warning with cause and remedy, except when an earlier acknowledged policy remains in force. Randomness may choose whether a warned risk materializes, but never invent an untraceable cause.

## Incident classes

Classes include medical call, disabled aircraft, fuel spill, security concern, vehicle conflict, runway excursion, gear or engine emergency, fire, severe-weather damage, and aircraft accident. Human outcomes are reported only as `No injuries`, `Injuries reported`, or `Fatalities reported`, with no bodies, blood, screams, or graphic animation.

## Response lifecycle

`Detected -> Alerted -> Resources dispatched -> Area protected -> Stabilized -> Investigated -> Cleared/repaired -> Reopened`.

Automation dispatches the nearest qualified resources. The player may set priority, close affected areas, change runway operations, request mutual-aid abstraction, and control reopening. Only affected facilities close unless safety boundaries require more.

The post-incident report explains causes, warnings, response, abstract outcome, financial/rating effects, and prevention steps. Repair is a construction project. Recovery aid from GS-15 prevents permanent failure even after aircraft loss.

## Visual reference

Use [VA-06 Weather and incident response](../30-content-and-assets/concept-art/06-weather-incident-response.png) to validate emergency routing, lifecycle visibility, abstract injury reporting, captioned radio, cause review, recoverable actions, and continued operation outside the closure. The corrected image intentionally does not offer the reciprocal end of a closed physical runway.

## Acceptance

Tests must prove warning provenance, seeded outcome, emergency-category check, dispatch and access, affected-only closures, abstract casualty presentation, investigation trace, repairs, recovery funding, and save/load at every lifecycle state. A content safety review is mandatory for every serious authored incident.
