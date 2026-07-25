# TS-06: 2D Rendering, UI, and Presentation Proxies

**Status:** Approved baseline  
**Owner:** Client presentation  
**Last updated:** 2026-07-25

## Outcome

Unreal renders a performant, crisp 2D airport while all authoritative state remains in the simulation. The initial platform is Windows desktop with mouse and keyboard.

## World presentation

The play world uses an orthographic camera, Paper 2D sprites/flipbooks, sprite-based effects, and 2D tile/shape layers where appropriate. Project settings must remove the blank template's 3D-oriented rendering assumptions: ray tracing and Substrate are not runtime requirements; lighting/material choices prioritize predictable 2D color and performance.

Presentation proxies are pooled objects keyed to `FSimEntityId`. A relevance service chooses visible/detail level from camera bounds, zoom, importance, and budget. Proxy transforms interpolate between fixed simulation samples but never feed coordinates back into simulation.

Zoom bands are:

- region: facilities, traffic summaries, alerts;
- airport: networks, buildings, aircraft, congestion;
- operations: aircraft, vehicles, groups, service states;
- detail: people, bags, doors, equipment, interior furniture.

Aggregation substitutes clustered icons or density visuals outside detail bands. It cannot alter logical quantities.

## Sorting and effects

Sorting follows CT-01 layers and explicit altitude/floor rules. Occlusion must not hide selected entities or safety alerts. Night and weather are 2D overlays/effects. Hit testing uses simulation footprints plus accessible selection padding, with deterministic tie-breaking defined by PX-02.

## UI architecture

CommonUI provides activatable screen layers, input routing, modal behavior, and reusable navigation patterns; UMG composes widgets. A view-model/adapter layer consumes immutable query snapshots and converts UI intents into typed commands. Widgets never retain raw simulation pointers.

Large lists are virtualized and query-paged. Map overlays use batched geometry or instancing, not one widget per cell/entity. UI updates on query revision and bounded refresh intervals, not every frame when values are unchanged.

## Performance behavior

Target is stable 60 FPS at 1920 x 1080 on the defined reference PC while simulating GS-20's load, with 2,000 visible logical agents, 150 aircraft, and 500 vehicles. When over budget, the system reduces cosmetic animation density, shadow/effect detail, and proxy count in that order; it never skips simulation or hides critical alerts.

## Visual-reference validation

The [Concept Art Reference Set](../30-content-and-assets/concept-art/README.md) defines comparison states for overview, build, terminal, timetable, turnaround, weather/incident, and progression surfaces. Implementation captures must be compared at equivalent zoom and state for world/UI balance, hierarchy, overlay readability, status redundancy, and tone.

The reference images do not authorize dimensional meshes or perspective. Translate their volume cues into Paper 2D sprites, flipbooks, 2D overlays, and restrained sprite effects. Written performance and accessibility requirements take precedence over visual density.

## Acceptance and sources

Tests must prove pooling reuse, relevance transitions, stable sorting, hit selection, query-only UI, 100–200% UI scale, captions, color-independent status, large-list virtualization, and graceful presentation degradation. A packaged scene must contain no required 3D gameplay asset.

Sources: [Paper 2D overview](https://dev.epicgames.com/documentation/unreal-engine/paper-2d-overview-in-unreal-engine?lang=en-US), [Paper 2D sprite import](https://dev.epicgames.com/documentation/unreal-engine/how-to-import-and-use-paper-2d-sprites-in-unreal-engine?lang=en-US), [CommonUI](https://dev.epicgames.com/documentation/unreal-engine/common-ui-plugin-for-advanced-user-interfaces-in-unreal-engine?lang=en-US).
