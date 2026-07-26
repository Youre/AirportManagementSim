# Phase 1.5 Component-Language Revision Architecture-Impact Review

**Status:** Approved for implementation
**Date:** 2026-07-26
**Change:** Reopen the Phase 1.5 visual gate and replace the primitive HUD component language

## Trigger

Owner review rejected the claim that the first Phase 1.5 presentation was
visually complete.
The Paper2D world and presentation plumbing are retained, but the runtime UI is
still materially unlike the approved concepts: flat rectangular surfaces,
default-looking controls, weak hierarchy, excessive diagnostic copy, and no
consistent rounded card or status treatment.

## Outcome and non-goals

This revision creates one reusable visual language for every current and future
HUD component. It changes shape, typography, spacing, density, grouping, and
presentation copy while preserving Phase 1 commands, simulation, save data,
economy, construction, offers, scheduling, services, and timing.

It does not add new gameplay, introduce 3D presentation, invent timetable
mechanics, or make widgets authoritative.

## Boundaries

- Add a focused `AMSimUITheme` module surface containing palette, type, rounded
  brushes, button variants, field styles, and spacing tokens.
- Keep `UAMSimRootScreen` as the native presenter and command adapter for this
  iteration. It consumes `FPhase1ViewState`; it does not access mutable state
  except through the existing typed command boundary.
- Recompose the root into a compact status header, action rail, objective rail,
  world-dominant center, and contextual decision tray.
- Keep required warnings actionable, but collapse routine cause/remedy,
  compatibility, ledger, rating, and service detail into short cards or
  contextual rows rather than permanent paragraphs.
- Update the component gallery from a text inventory to actual instances of
  the shared production primitives.

## Compatibility and rollback

No save schema, deterministic state, package identity rule, or content
definition changes. Rollback restores the previous `AMSimRootScreen`,
`AMSimComponentGallery`, and theme helper without data migration.

## Verification obligations

- Build `AMSimEditor` and run focused presentation tests before full automation.
- Capture the four equivalent Phase 1 states at 100% first and inspect them
  side by side before running the full scale matrix.
- Confirm the 175–200% compact transition preserves the world and essential
  actions.
- Re-run the Phase 1 command journey, cook/package gate, cursor check, and
  forbidden editor/MCP dependency scan after visual acceptance.
