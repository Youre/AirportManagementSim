# Phase 4.5 Architecture-Impact Review

**Status:** Passed with mandatory controls  
**Date:** 2026-07-26  
**Scope:** CT-05 and PI-07 presentation remediation

## Decision

Phase 4.5 is presentation-only. It adds no simulation domain, save schema,
network dependency, or 3D pipeline. Existing Phase 1–4 commands, records,
events, queries, checksums, and save state remain authoritative.

## Ownership

- `AMSimSimulation` remains unchanged unless a missing immutable query field is
  proven necessary; any such field must be derived from existing state.
- `AMSimGameplay` retains content resolution and lifecycle/save orchestration.
- `AMSimUI` owns new focused visual presenters, view models, Paper2D proxy
  layers, camera modes, and local selection.
- `AMSimEditor` owns deterministic asset import, provenance, cooking, and
  content validation.
- `AMSimTests` owns requirement mapping, geometry, isolation, responsive, and
  package assertions.

## Source organization

`AMSimRootScreen.cpp`, `AMSimTerminalView.cpp`, and
`AMSimRegionalOperationsView.cpp` are already large. New content surfaces must
use focused files and reusable primitives. No touched source file may exceed
2,000 lines.

## Persistence

Selected visual tab, camera mode, inspector expansion, hover/focus, proxy
assignment, animation phase, Figma node, and screenshot state are transient.
Phase 4.5 does not increment schema 5.

## Rendering and scale

New world assets remain Paper2D. Pooled proxies and bounded presentation
refreshes remain mandatory. Normal layouts keep the world dominant; timetable
and capability map are approved full-screen planning exceptions. High-scale
layouts use drawers and scroll surfaces without shrinking required content.

## Content/package boundary

All runtime assets require serialized references, provenance, and direct
Shipping inventory evidence. Runtime string loading, editor/MCP/toolset
leakage, external unreviewed aircraft, and required 3D candidates remain
release failures.

## Mandatory controls

- No presentation coordinate feeds simulation state.
- No text symbol or blank box substitutes for a required asset.
- No new authoritative state solely to make a screenshot resemble concept art.
- Every spatial explanation has a world layer and a query-backed inspector.
- Equivalent-state metadata rejects wrong lifecycle state, scale, or zoom.
- Full Phase 1–4 deterministic, save, package, offline, and mouse/input gates
  re-run after integration.

With these controls, CT-05 fits the existing architecture without reopening the
completed simulation phases.

