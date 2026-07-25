# GS-16: Ratings and Satisfaction

**Status:** Approved baseline  
**Owner:** Performance feedback  
**Last updated:** 2026-07-25

## Outcome and scope

Players can see what their airport does well, why a score changed, and what action could improve it. This specification owns airport rating, passenger and tenant satisfaction, operational scorecards, history, and offer effects.

## Measures

The visible airport rating has five components, each 0–100:

- safety and readiness;
- operational reliability;
- passenger or customer experience;
- tenant and operator relationships;
- access, cleanliness, and amenities.

The overall rating is a weighted mean using GS-20 defaults. Airports without passengers receive a neutral excluded passenger component rather than a penalty; cargo, GA, and flight-school equivalents use customer/service evidence appropriate to their path.

Passenger satisfaction is computed at journey completion from time confidence, required-process waits, comfort needs, information, missed connections, baggage result, and disruption handling. Tenant satisfaction follows GS-12. Operational reliability separates airport-controllable and external causes.

## Explainability

Every score change creates a reason contribution with source, magnitude, time window, and affected group. Panels show current value, trend, largest positive and negative drivers, sample size, and one suggested improvement. A score based on too little evidence is labeled "building a picture" rather than presented as certainty.

Recent performance has more influence than old performance, but a single ordinary mistake cannot collapse the rating. Serious warned-risk incidents have a larger bounded effect and a visible recovery path.

## Effects

Ratings influence offer variety, tenant interest, contract rewards, and achievements. They do not lock the player out of basic recovery offers or make a save unwinnable. Decorations affect ratings only through disclosed functions such as comfort, wayfinding, noise screening, or scenery.

## Acceptance

Tests must prove path-neutral scoring, weighted aggregation, sample confidence, bounded incident effects, driver explanations, recovery over time, offer influence, and save/load history. Every displayed score must be reproducible from stored reason contributions.

