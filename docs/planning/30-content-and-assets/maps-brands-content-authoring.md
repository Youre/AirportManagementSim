# CT-04: Maps, Brands, Text, and Content Authoring

**Status:** Approved baseline  
**Owner:** Content authoring  
**Last updated:** 2026-07-25

## Outcome and scope

Designers can add curated regions, facilities, objectives, operators, events, and readable text through validated data without changing simulation code. This specification owns content packages, authoring conventions, text readiness, validation, and release manifests.

## Content packages

Required packages are:

- maps and regional profiles;
- facility, network, vehicle, staff-role, and service definitions;
- aircraft and fictional operators;
- contracts and offer templates;
- objectives, achievements, guided steps, and contextual help;
- weather profiles and special events;
- names, brands, signage, radio phrases, and UI text;
- balance profiles and compatibility matrices.

Content references stable IDs and gameplay tags defined in TS-04. Display names are never keys. Packages declare schema version, owner, phase, dependencies, localization keys, and rights/provenance status.

## Curated maps

Each map ships with a manifest covering purchasable parcels, external transport ports, terrain restrictions, climate, daylight, prevailing winds, scenery, starter-airfield validation, supported specializations, and performance test points. Every map must support at least three specializations; the initial temperate map must support all six.

## Text and brands

Player-facing text is stored by stable localization key even for an English-only initial release. Body text targets short sentences and basic reading while retaining authentic aviation terms. Help entries pair term, plain explanation, situation, and optional example.

Names and brands are fictional and screened for collision, inappropriate meaning, accidental real-operator imitation, and duplicates. Generated passenger names use curated local tables and do not encode protected-trait gameplay effects.

## Validation and release

Editor validation checks references, schema, localization keys, type ranges, compatibility coverage, provenance, duplicate names/IDs, forbidden real branding, inaccessible text, achievement path neutrality, and map start viability. A release manifest records exact content versions and hashes so save migrations and test fixtures remain reproducible.

## Acceptance

Content-only changes must be able to add one operator, objective, event, and facility without C++ modification. A full validation run must report zero broken references, missing player text, unreviewed rights items, or maps that cannot complete their declared starter loop.

