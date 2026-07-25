# TS-04: Data Assets, Registries, and Gameplay Tags

**Status:** Approved baseline  
**Owner:** Static data  
**Last updated:** 2026-07-25

## Outcome

Static content and tuning are discoverable, validated, versioned, and loadable without embedding content rules throughout code.

## Ownership model

Primary Data Assets own authored definitions with stable Primary Asset IDs. Data Registries provide typed lookup and controlled overrides for frequently queried tabular data. Gameplay Tags classify capability and compatibility; they do not replace strongly typed IDs or numerical fields.

Definition families include maps, facilities, network pieces, aircraft, operators, vehicles, staff roles, services, contracts, passengers, cargo, weather, events, objectives, achievements, text/help, audio voices, visual assets, and balance profiles.

Every definition includes schema version, stable content ID, display-text keys, tags, owning phase, dependencies, provenance/rights state where applicable, and validation status. Referencing a missing or incompatible definition fails validation; runtime does not guess by display name.

## Tags and compatibility

Tag roots are centrally registered, including `Aircraft.Role`, `Aircraft.Size`, `Facility.Type`, `Network.Type`, `Service.Type`, `Staff.Role`, `Zone.Type`, `Cargo.Class`, `Weather.Hazard`, `Capability.Path`, and `Event.Type`. Tags express membership. Compatibility requiring ranges—wingspan, length, capacity, weather minimum, pavement load—uses typed fields and rule evaluators.

## Loading and versioning

The Asset Manager scans declared primary asset types. A map manifest declares the content bundles needed before entering play; optional details may stream later but cannot affect deterministic availability after the load boundary. Save snapshots record stable content IDs and the release manifest hash.

Data-only tuning overrides require matching schema and validation. User-mod loading is out of scope for the initial release.

## Validation

Editor and command-line validation checks IDs, references, cycles, tag roots, ranges, compatibility coverage, text, assets, provenance, save migration aliases, and phase availability. The build fails on errors; warnings require an owner and disposition before a release candidate.

## Acceptance and sources

Tests must resolve every referenced definition from a clean packaged manifest, detect duplicate IDs and cycles, apply a known override deterministically, and migrate an aliased ID.

Sources: [Asset Management](https://dev.epicgames.com/documentation/en-us/unreal-engine/asset-management-in-unreal-engine), [Data Registries](https://dev.epicgames.com/documentation/en-us/unreal-engine/data-registries-in-unreal-engine).

