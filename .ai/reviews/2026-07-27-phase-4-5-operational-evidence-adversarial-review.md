# Phase 4.5 Operational Evidence Adversarial Review

**Status:** Passed with completion gates remaining  
**Date:** 2026-07-27  
**Scope:** the remaining schematic visual-content requirements

## Findings

### Construction activity could be decorative rather than authoritative

Construction proxies are gated by `EConstructionStage`: delivery vehicles show
only after funding, workers and protected zones show while building, inspection
markers show during inspection, and every work proxy clears on cancellation or
completion. Existing query-backed stage copy remains the textual evidence.

### Turnaround density could imply manual vehicle control

Vehicles, equipment, safety zones, and approach arrows are presentation
proxies derived from service task state. The timeline explicitly labels
automatic dispatch and safe assignment; no steering or direct-drive command is
added.

### Color could be the only conflict or safety signal

Construction uses localized striped geometry and concise labels. Turnaround
uses authored protection-zone, cone, and direction-arrow sprites in addition to
color. Timetable risk remains on the affected time card with `HIGH RISK` text
and a bounded weather window.

### Requirement status could be promoted from code inspection alone

A requirement moves to `present` only after its runtime state is captured in
the equivalent comparison board and the relevant automation passes. Existing
contract identity and specialization thumbnails must be visibly legible in
their matching captures, not merely cooker-visible.

## Remaining risks

- Normal/compact five-scale gallery checks, clean Shipping packaging, manual
  mouse/focus/pan/zoom checks, and owner approval remain Phase 4.5 completion
  gates.
- Cargo remains an honest future-locked destination until its owning gameplay
  phase exists.

