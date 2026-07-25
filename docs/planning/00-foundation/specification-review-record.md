# Implementation Planning Review Record

**Status:** Approved  
**Review date:** 2026-07-25  
**Scope:** Complete implementation-planning specification set

## Architecture-impact review

Affected planned modules are `APSim`, `APSimSimulation`, `APSimGameplay`, `APSimUI`, `APSimEditor`, and `APSimTests`. Ownership is explicit in TS-01. The cross-module public contracts are stable IDs, typed commands, immutable events, revisioned query snapshots, static definitions, and versioned save snapshots.

The design preserves all project invariants:

- Unreal Engine remains the target;
- shipped gameplay and presentation remain 2D and top-down;
- MCP remains an editor-only inspection/automation tool;
- AirplaneGame remains read-only until an approved manifest and rights review;
- simulation rules remain separate from presentation.

The architecture intentionally replaces the blank-template assumption with a modular deterministic simulation. There is no code or save data to migrate yet. Once implementation starts, every authoritative schema change requires a TS-05 migration fixture. Rollback is specification revision before implementation and source-control reversion after implementation; player saves must never be rolled back without a supported migration/recovery path.

Required review roles during implementation are gameplay owner, simulation/architecture owner, UI/accessibility reviewer, content/aviation accuracy reviewer, and save/test owner. Incident content additionally requires a child-safety review.

## Adversarial review

### Risks found and controls

| Risk | Control in the approved set |
| --- | --- |
| Scope expands into a global airline or route simulation | GS-04 keeps the wider network as background demand |
| Passenger play becomes the mandatory endgame | GS-12 and PI-01 require all six Major paths |
| 10,000 named agents become 10,000 ticking Actors | TS-02 records plus TS-06 pooled relevance proxies |
| Accelerated time changes outcomes | Fixed-step processing, no skipped steps, replay checksums |
| A save is corrupted by interrupted async write | Temporary write, validation, atomic replace, previous-good backup |
| Content IDs or balancing break old saves | Stable IDs, manifest hash, aliases, ordered migrations |
| Serious accidents feel random or depict harm | GS-18 requires warned traceable risk and abstract outcomes |
| Recovery aid can still leave no playable airport | GS-15 preserves or restores a basic airfield loop |
| MCP or experimental tools leak into runtime | TS-01/07/09 and package audits exclude editor dependencies |
| Local speech fails or becomes a network dependency | CT-03 captions-first provider abstraction and silent fallback |
| Existing aircraft art is copied without rights/provenance | CT-01 import manifest gate and checksum validation |
| Network closures produce invisible cascading failures | GS-02 structured reasons and affected-component invalidation |
| Specification count hides missing integration | PI-01 vertical slices and PI-02 end-to-end scenarios |

### Remaining known decisions

- Reference-PC hardware must be recorded in Phase 0 before performance budgets become release gates.
- Aircraft model data, model-name usage, source sprite rights, and fictional livery review remain per-content approvals.
- Unreal's TextToSpeech provider must pass quality and packaging evaluation; captions and silent fallback are already normative.

No contradiction was found between `ideas.txt`, the universal constraints, the feature ownership map, and the phased roadmap. No implementation code is included in this planning change.

## Concept-art review addendum

**Review date:** 2026-07-25

Seven generated visual references were reviewed against the approved architecture and feature behavior before being linked into the specifications.

- Architecture impact: the images add no runtime dependency, asset-import approval, UI schema, or 3D authorization. They are planning-only PNG evidence stored under `docs/planning/30-content-and-assets/concept-art/`.
- Authority control: the concept-art index explicitly places written specifications above imagery and prohibits copying incidental generated values, geometry, text, or dimensional shading.
- Accessibility control: mapped comparisons evaluate icon/pattern/text redundancy, readable hierarchy, overlays, captions, and world/UI balance rather than color matching alone.
- Adversarial finding corrected: the first turnaround image implied manual vehicle control. It was regenerated to show priority changes and safe reassignment.
- Adversarial finding corrected: the first incident image offered reciprocal Runway 09 while the physical Runway 27 was closed. It was regenerated to offer hold, divert, and tow actions.
- Remaining risk: generated aircraft geometry, perspective-like shading, small text, and facility layouts can appear more authoritative than they are. CT-01, TS-06, and the concept-art index explicitly classify these details as non-normative.

The reference set is suitable for implementation comparison gates only when opened alongside its owning specification.
