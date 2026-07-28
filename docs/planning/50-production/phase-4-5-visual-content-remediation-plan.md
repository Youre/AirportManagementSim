# PI-07: Phase 4.5 Visual Content Remediation

**Status:** Complete; accepted baseline with continuing visual polish debt  
**Owner:** Presentation engineering and visual design  
**Approved:** 2026-07-26  
**Normative requirements:** CT-05  

## Goal

Replace the current text-led schematic presentation with the complete visible
content required by VA-01 through VA-07, while preserving all verified Phase
1–4 simulation, save, performance, offline, and package behavior.

Phase 4.5 is a cross-phase presentation gate. It does not open Phase 5.

## Architecture

- Add focused presenter widgets rather than extending
  `AMSimRootScreen.cpp`, `AMSimTerminalView.cpp`, or
  `AMSimRegionalOperationsView.cpp` beyond the repository size limit.
- Derive presentation models from immutable Phase 1–4 queries and read-only
  state records. No new authoritative simulation state is permitted.
- Retain the accepted Riverbend palette and rounded component language.
- Replace text-only explanations with composed visual structures and world
  counterparts.
- Store all art through cooker-visible UObject references or presentation
  descriptors. Runtime string asset loading remains prohibited.
- Keep new generated or authored assets project-owned, editable, provenance
  recorded, and 2D.

## Work packages

### P45-00 — requirements and validation baseline

- Adopt CT-05 and its stable requirement/component IDs.
- Create a machine-readable coverage manifest.
- Correct the visual-review vocabulary: layout/readability pass does not imply
  concept-content parity.
- Build seven equivalent-state comparison fixtures and reject differently
  scoped evidence.
- Create a supplemental Figma validation artifact when the Figma MCP is
  available; repository requirements remain authoritative.

### P45-01 — overview and build content

- Implement VC-C02 through VC-C07.
- Add mature overview, tool rail, activity rail, selection inspector, build
  palette, placement overlay, localized conflict/closure, validation rail, and
  construction-stage world treatment.
- Preserve the distinct new-airport creation state.

### P45-02 — turnaround content

- Implement VC-C14 and VC-C15.
- Add a close-operations camera mode, selected-aircraft inspector, recognizable
  service vehicles/workers/equipment, safe-position overlays, and a dependency
  timeline.
- Keep dispatch automatic and simulation-owned.

### P45-03 — terminal content

- Implement VC-C08 through VC-C10.
- Replace anonymous rectangles with reviewed room/door/furniture/equipment,
  people, bag, vehicle, staff, and route assets.
- Keep the cutaway and routes query-driven and pooled.

### P45-04 — timetable and regional content

- Implement VC-C11 through VC-C13.
- Use authoritative day/minute/gate data to position cards in a real timetable
  grid.
- Add fictional operator/aircraft identity, localized risk, affected forecast
  windows, and adjacent feasibility evidence.

### P45-05 — weather and incident content

- Implement VC-C16 through VC-C18.
- Add reviewed weather treatments, a clear affected aircraft/area, localized
  closure, emergency route, response vehicles, protection zone, and lifecycle
  visualization.
- Preserve readable captions and age-appropriate outcomes.

### P45-06 — progression content

- Implement VC-C19 through VC-C21.
- Add the six-path capability map, capability bands, three suggested
  objectives, selected-path inspector, combined-path explanation, and honest
  future-locked nodes.
- Do not add cargo or Major-airport simulation.

### P45-07 — shared art kit and integration

- Implement VC-C22 with coherent icons, airport props, terminal props, people,
  staff, vehicles, markings, patterns, operator identity, and world thumbnails.
- Record generator/source, prompt or authoring method, dimensions, cleanup,
  checksum, destination, import settings, and reuse approval.
- Re-run all presentation, cooking, offline, and regression gates.

## Verification

- Unit coverage for every presentation model and requirement status transition.
- Automation proving timetable geometry, overview activity mapping, service
  dependencies, progression equality, route layers, incident localization,
  proxy bounds/reuse, and presentation isolation.
- Development component/content gallery at all five scales.
- Primary equivalent-state boards for VA-01 through VA-07 plus before/after
  bounded iterations.
- Manual cursor, hover, focus, pan/zoom, selection, camera-mode, caption,
  drawer, modal, and dismissal checks.
- Full Phase 1–4 automation, deterministic rendered fixtures, schema-5
  save/load, clean Development and Shipping packages, direct IoStore content
  inventory, zero forbidden dependency/runtime-load/3D matches, and short
  offline packaged smoke.

## Completion

Phase 4.5 completes only when all CT-05 requirements are present or explicitly
future-locked, all seven equivalent-state boards receive owner visual
acceptance, and no verified Phase 1–4 behavior regresses.

As of 2026-07-28, all 82 implementable requirement and component assessments
are `Present`; VC-PRG-08 is the sole approved `Future-locked` assessment. The
five-scale gallery, clean package, direct IoStore, regression, and rendered
evidence are recorded in the verification log. The packaged manual interaction
checklist passes. Owner review of the seven boards was the final completion
gate.

On 2026-07-27 the owner accepted Phase 4.5 as the technical and visual-content
baseline for continued development while explicitly retaining aesthetic
refinement as continuing visual debt. That decision satisfies the owner-review
gate and opens Phase 5 without claiming that later polish is exhausted.
