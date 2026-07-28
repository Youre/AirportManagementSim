# Phase 4.5 Selection, Boundary, and Weather Adversarial Review

**Status:** Passed with remaining visual gates  
**Date:** 2026-07-27  
**Scope:** Selected-entity variants, controlled/baggage transitions, identity,
wet surfaces, and deterministic proof capture

## Findings

### Facility selection could be cosmetic

The facility tab now changes identity art, title, capacity/service evidence,
button state, and the Paper2D focus ring. A companion 1920x1080 capture proves
the terminal rather than the aircraft is selected. Neither target position is
written to simulation.

### Exception treatment could imply a nonexistent incident

The exception branch is always spatially discoverable but explicitly reports
`CLEAR` when no authoritative exception exists. It changes to the active
coral treatment only when a bag record enters `Exception`; the command remains
disabled otherwise.

### Controlled security could still rely on cyan

Partition and secure-door assets now form the transition, while
`CONTROLLED DOOR` and the query-backed security card provide concise text
evidence. Invalid topology switches the authored boundary assets to the
critical treatment.

### Wet polish could obscure response evidence

Wet runway, taxiway, and apron layers sit below closure hatching, aircraft,
route connectors, direction markers, and vehicles. The recapture preserves
all incident actions and text at normal scale.

### Proof flags could contaminate gameplay

The overview/facility flags are development-only launch evidence. They change
transient widget/presenter state after the deterministic fixture completes.
The added two-frame wait only settles presentation before capture and does not
advance commands, create records, or alter persistence.

## Remaining risks

- Active baggage-exception appearance is code- and state-backed but the current
  equivalent VA-03 fixture truthfully captures the clear state.
- Tool-specific drawers, richer needs pictograms, higher world density, and
  bounded atmospheric depth remain visual-quality work.
- Five-scale gallery, clean package, manual input/mouse checks, and owner
  approval remain open completion gates.

