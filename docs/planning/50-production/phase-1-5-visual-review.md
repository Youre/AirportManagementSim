# Phase 1.5 Visual Review

**Review date:** 2026-07-26  
**Implementation state:** technically verified; final owner visual acceptance pending

## Evidence set

The ignored working evidence is under:

- `AMSim/Saved/VisualReview/Phase1.5-Mockups`;
- `AMSim/Saved/VisualReview/Phase1.5-Comparisons`;
- `AMSim/Saved/Phase1/Scale-Phase15Final`.

`scripts/ui/New-VisualComparison.ps1` creates an equivalent-state board and
worksheet. `scripts/ui/Invoke-Phase15ScaleMatrix.ps1` captures the full Phase 1
journey at 1920x1080 and 100%, 125%, 150%, 175%, and 200% application scale.

## Equivalent-state assessment

### P15-01 — New airport overview

The implementation now reserves the center for the world, uses stable dark
slate rails, and keeps airport identity, time, funds, objective, and offer
status in persistent regions.

Highest-impact remaining differences:

1. the concept has a curated river/road/forest parcel while the baseline uses
   one seamless grass surface;
2. the concept uses an icon rail and illustrated location card;
3. the implementation exposes more Phase 1 diagnostic text than the concept.

Intentional difference: until world-building tools exist, the empty parcel
uses deterministic Paper2D terrain rather than inventing decorative simulation
features.

### P15-02 — Construction proposal and progress

The committed runway, taxi connection, stand, access, hut, and windsock now
exist in the Paper2D world. A contextual construction card identifies the
connected package, funding, and current stage.

Highest-impact remaining differences:

1. the concept has selection handles, facility labels, and a construction
   confirmation tray;
2. the baseline facilities use simple editable sprites rather than final
   illustrated props and markings;
3. construction progress is textual rather than a staged material/crew
   animation.

Intentional difference: construction geometry is presentation-only and never
feeds positions back into the deterministic simulation.

### P15-03 — Offer, compatibility, and timetable

The offer state now receives a central contextual card with operator, aircraft
type, recommended slot, stand, turnaround, compatibility, and reward. The
persistent operations rail retains the actual Phase 1 actions.

Highest-impact remaining differences:

1. the concept has a full timetable grid rather than one exact five-minute
   recommendation;
2. compatibility is summarized in prose instead of a large checklist card;
3. the operator has no final crest or aircraft icon.

Intentional difference: Phase 1.5 does not add Phase 4 timetable mechanics or
new scheduling data solely to imitate the mockup.

### P15-04 — Aircraft turnaround

The approved Cessna source is present as a fictional yellow Riverbend aircraft
on Stand A1, with protected selection/service markers and a contextual
turnaround card. The aircraft remains presentation-only and selects from 16
reviewed headings.

Highest-impact remaining differences:

1. the concept has service-worker illustrations, equipment, and animated
   progress bars;
2. the stand/hut/marking kit remains deliberately simple;
3. the concept uses a closer management camera for the aircraft.

Intentional difference: service progress remains query-backed text until the
service-animation work owns representative workers, vehicles, and progress
motion.

## Bounded iterations completed

1. Rebalanced the shell from proof-map panels to a world-dominant viewport.
2. Moved runway, taxiway, stand, access, hut, windsock, construction, selection,
   service, and aircraft presentation into the Paper2D world.
3. Added state-specific construction, offer/timetable, and turnaround context
   cards without changing simulation behavior.
4. Increased management-scale stand and aircraft legibility while preserving
   the source asset's 32 pixels-per-meter definition.
5. Replaced the 175–200% stacked layout with compact, mutually exclusive
   objective/operations drawers and a horizontally scrollable control strip.
6. Reduced compact header/context density so text remains readable without
   consuming the world.

## Technical verification

- `AMSimEditor` builds on Unreal Engine 5.8.0.
- The project audit passes with the approved Phase 1.5 content review.
- All 27 AMSim automation results complete with zero failures, skipped tests,
  or in-process tests.
- The deterministic rendered journey retains checksum
  `6583174326702355518`.
- All five scale captures pass with zero 8x backlog.
- Manual PIE inspection confirmed a visible OS cursor, hover/click behavior,
  focus restoration, and progression through the representative states.
- A clean Shipping cook produces 267 packages with no missing presentation
  dependency.
- The production root Widget Blueprint and presentation assets are present in
  the cooked registry; the development component gallery is absent.
- The Shipping runtime remains alive through the five-second launch check.
- Forbidden Shipping file, text, and receipt matches are all zero.

## Acceptance boundary

The implementation and validation loop are ready for owner review. Phase 1.5
must not be marked complete until the owner accepts the four final comparison
boards. Later illustrated environment detail, operator identity art, timetable
grid depth, and service-worker animation remain bounded follow-on polish rather
than hidden Phase 1.5 completion claims.
