# GS-05: Airspace, Runways, and ATC

**Status:** Approved baseline  
**Owner:** Airside operations  
**Last updated:** 2026-07-25

## Outcome and scope

Players see an active, authentic-feeling traffic pattern without manually issuing every clearance. This specification owns runway configuration, numbering, approach aids, airspace queues, automated ATC, separation, policies, and overrides.

## Runways and aids

- Runway numbers derive from magnetic heading rounded to the nearest ten and expressed as 01–36. Reciprocal ends differ by 18; parallel runways use L/C/R suffixes.
- Each end defines declared length, surface, lighting, approach type, traffic direction, thresholds, holding points, and operating weather minima.
- PAPI or VASI is a selectable visual approach-aid facility where compatible. It improves approach capability and presentation but does not replace instrument-approach requirements.
- Runway states are `Closed`, `Available`, `Active arrival`, `Active departure`, or `Mixed`. The player may close individual ends or set preferences.
- Wind, visibility, precipitation, surface condition, noise policy, construction, and incidents affect the recommended configuration.

## Automated ATC

ATC automatically sequences inbound, outbound, pattern, and crossing traffic. It issues voiced and captioned calls for clearance, runway assignment, hold, go-around, landing, taxi handoff, and takeoff. The player controls policies: preferred runway, mixed-use permission, arrival/departure priority, training-pattern limit, emergency priority, and acceptance of modest tailwind.

Players can override an individual runway assignment, order a hold, prioritize a flight, approve a crossing, close a runway, or initiate a go-around. Unsafe commands are blocked; risky but legal commands require confirmation and become traceable risk inputs.

## Kinematic movement and separation

Aircraft follow authored kinematic paths and performance envelopes; full aerodynamic physics is out of scope. Airspace uses entry fixes, holding positions, approach paths, departure paths, and traffic-pattern legs. Separation rules consider wake class, runway occupancy, intersecting paths, and emergency status. Only one authority can reserve a runway segment for a conflicting movement at a time.

If separation cannot be maintained, automation delays clearance, extends downwind, holds, or commands a go-around. It never resolves congestion by overlapping aircraft or teleporting them.

## Weather and failure behavior

ATC consumes current and forecast operating categories from GS-17. A runway recommendation changes before unsafe thresholds where forecast confidence allows. Sudden deterioration triggers holds, go-arounds, diversions, or closure. Approach-aid or lighting outages may raise minima; infrastructure does not randomly degrade, so such outages occur only through incidents or authored events.

## Information

The runway panel shows active end, wind component, declared distances, queue, occupancy, approach capability, closure reason, and recommendation. The airspace overlay shows flight labels, intended paths, holds, and separation warnings without requiring aviation-chart expertise.

## Acceptance

Tests must verify runway numbering including reciprocal and parallel cases, wind recommendation, reservation exclusivity, wake separation, crossing protection, go-around, emergency priority, weather-minima response, override logging, and identical sequencing from identical inputs. A stress fixture must process the supported traffic target without per-aircraft Tick ownership.

