# Visual Baseline Product Audit

**Reference:** VA-01 airport overview and HUD
**Implementation:** packaged `VisualBaseline` slot at 1920×1080, 100% UI scale
**Scope:** reusable visual test state and the persistent root HUD shell

## What already works

- The packaged build can now load a mature deterministic airport instead of replaying airport creation and construction for every visual check.
- The Paper2D world contains a recognizable terminal, stands, service buildings, roads, parked aircraft, vehicles, trees, and active selection feedback.
- The Riverbend navy/cyan/amber palette, rounded cards, high contrast, visible cursor, and caption evidence are consistent with the approved direction.
- Left-side tool destinations already use project-owned icons and hover flyouts rather than permanent labels.

## UX and design findings

### P1 — persistent activity detail consumes the world

The implementation dedicates 320 px to objectives, status, living-airport diagnostics, and many small actions. VA-01 uses a compact alerts/flights/projects rail and opens detail contextually. This is both a visual mismatch and an information-architecture issue: permanent detail competes with the airport, while several disabled actions look available enough to demand inspection.

### P1 — simulation controls are separated from simulation status

Time state is in the header while play and speed controls are mixed into a footer containing save/load, screen destinations, window mode, and help. The concept groups the status and its controls, which is faster to scan and leaves the bottom region available for contextual inspection.

### P1 — no VA-01 bottom selection inspector in the root view

The selected aircraft is highlighted in the world, but its identity, readiness, services, and contextual actions do not appear in the root view's bottom region. Some of that information exists on other screens and in the right diagnostic card, but that is not equivalent to the required spatial selection pattern.

### P2 — header and footer are too tall and text-heavy

The brand, weather/status pill, and economy pill wrap at the baseline viewport. The footer presents thirteen similarly weighted controls plus two text rows. This creates a prototype/debug-console rhythm rather than the compact operational HUD shown in VA-01.

### P2 — readability is inconsistent with the written minimum

Several persistent labels use 11–14 point equivalents and all-caps strings. Contrast is strong, but the smaller sizes are below the project's 18 px-equivalent body-text requirement and become harder to scan at distance.

## First bounded remediation

1. Preserve a deterministic `VisualBaseline` save and support historical save metadata through the current migration path.
2. Collapse persistent activity detail behind a right icon rail for alerts, flights, and projects; retain the complete existing drawer as the initial drill-down.
3. Move pause and speed controls beside the header status, reduce multi-line brand chrome, hide locked destinations, and remove the disabled current-destination footer control.
4. Keep the real Paper2D world renderer authoritative and remove unused hidden UMG runway/taxi/stand/aircraft placeholders.

The bottom selection inspector and further world composition/density tuning remain the next VA-01 remediation system after this shell pass.
