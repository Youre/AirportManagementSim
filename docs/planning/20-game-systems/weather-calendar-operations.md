# GS-17: Weather, Calendar, and Operating Conditions

**Status:** Approved baseline  
**Owner:** Environment  
**Last updated:** 2026-07-25

## Outcome and scope

Weather is forecast, meaningful, and operationally legible. This specification owns game calendar, daylight, wind, visibility, precipitation, temperature, operating categories, forecasts, runway condition, and weather response.

## Time and calendar

Each save has local date, time, weekday, season, sunrise, and sunset. The calendar repeats fictional years without real-world holidays unless an authored event uses them. Time advances only while the save is loaded and unpaused; there is no offline progression.

## Weather model

Current conditions include wind direction/speed/gust, visibility, cloud category, precipitation type/intensity, temperature, and runway surface condition. Supported categories are clear, cloudy, rain, thunderstorm, fog/low visibility, snow, ice risk, and strong/gusty wind. Region and season govern plausible transitions.

Weather evolves from a seeded timeline generated far enough ahead to support forecasts. It is not rerolled on load. A forecast exposes:

- reliable near-term hourly detail;
- broader later trend and confidence;
- highlighted operational risks;
- likely runway recommendation and deicing demand.

The airport weather display uses authentic values and plain explanations. A compact METAR-style line may be shown alongside, not instead of, the readable display.

## Operational effects

Wind affects runway choice and aircraft limits. Visibility/cloud affect approach eligibility. Rain/snow/ice affect runway occupancy, braking category, service travel, and deicing need. Temperature affects deicing and passenger comfort. Lightning or severe cells may suspend exposed ramp work.

Effects use type-specific limits and disclosed thresholds. Weather does not directly damage facilities through random wear. Severe authored weather may create incident risk only through GS-18's warning and traceability rules.

## Visual reference

[VA-06 Weather and incident response](../30-content-and-assets/concept-art/06-weather-incident-response.png) is the reference for pairing current conditions, a confidence-bearing forecast, runway effects, weather overlay, and readable captioned operations. Its values and timeline are illustrative.

## Acceptance

Tests must prove seeded continuity, seasonal plausibility, forecast confidence, runway recommendation, aircraft-limit differences, deicing trigger, ramp suspension, daylight, pause behavior, and save/load without reroll. Every weather-caused restriction must identify the observed condition and applicable limit.
