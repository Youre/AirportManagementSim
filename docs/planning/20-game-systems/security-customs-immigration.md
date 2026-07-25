# GS-10: Security, Customs, and Immigration

**Status:** Approved baseline  
**Owner:** Controlled passenger processing  
**Last updated:** 2026-07-25

## Outcome and scope

Players build comprehensible controlled flows without turning the game into a punitive inspection simulator. This specification owns sterile-area boundaries, passenger screening, bag-screening outcomes, immigration, customs, inspection selection, staffing, throughput, and privacy posture.

## Zones and transitions

Terminal cells may be landside, sterile departures, sterile arrivals, domestic, international, or restricted staff space. Crossing between zones requires a typed controlled facility. Doors cannot create an implicit bypass. Invalid boundaries block facility opening and highlight the breach.

## Security

Passenger screening consists of document/boarding-pass check, queue, screening lane, and optional secondary inspection. Bag screening connects to GS-09. Lanes have staff, equipment, throughput, accessible handling, and open/closed state. A passenger cleared for a zone retains that state until leaving through an invalidating boundary.

Inspection outcomes are abstract and age-appropriate: cleared, needs another check, prohibited item surrendered, or denied access. The game does not model weapon construction, evasion tactics, invasive procedures, or graphic threats.

## Border and customs

International journeys may require passport control and customs. Immigration validates itinerary eligibility through fictional documents; customs uses declaration choice plus a policy-driven inspection rate. The player can set lane allocation and broad inspection posture within safe bounds, but cannot discriminate by protected personal traits.

## Capacity and incidents

Queues, staff availability, lane equipment, and arrival surges determine throughput. Closing a checkpoint reroutes only if another valid controlled path exists. A discovered concern pauses the affected person, bag, or lane and may request airport police; it does not automatically close the entire airport.

## Information and accessibility

Panels show required process, lane state, wait forecast, staffing, accessibility, and the reason a passenger cannot proceed. Captions and icon-plus-text feedback are mandatory. Passenger data is fictional, local to the save, and never exported.

## Visual reference

Use [VA-03 Terminal and passenger flow](../30-content-and-assets/concept-art/03-terminal-passenger-flow.png) to validate continuous controlled boundaries, process order, checkpoint visibility, non-color zone encoding, and accessible routing. The illustration does not define actual security equipment or throughput.

## Acceptance

Tests must prove zone integrity, no door bypass, screening-state invalidation, secondary inspection, international arrival/departure flows, customs selection independent of protected traits, lane closure rerouting, accessible lanes, and save/load of controlled-state passengers. Content review must confirm child-appropriate language and no procedural harm depiction.
