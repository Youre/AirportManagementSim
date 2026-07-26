# Phase 1.5 Visual Review

**Review date:** 2026-07-26  
**Implementation state:** accepted and complete

The owner rejected the first runtime comparison set as visually too primitive,
specifically citing flat surfaces, missing rounded edges, excess copy, and weak
consistency with the approved concepts. The corrective evidence below was
accepted on 2026-07-26 as the reusable Phase 1.5 visual baseline.

## Evidence set

The ignored working evidence is under:

- `AMSim/Saved/VisualReview/Phase1.5-Mockups`;
- `AMSim/Saved/VisualReview/Phase1.5-ComponentLanguage`;
- `AMSim/Saved/VisualReview/Phase1.5-Comparisons-ComponentLanguage-Candidate`;
- `AMSim/Saved/Phase1/Scale-Phase15-ComponentLanguageCandidate`.

`scripts/ui/New-VisualComparison.ps1` creates an equivalent-state board and
worksheet. `scripts/ui/Invoke-Phase15ScaleMatrix.ps1` captures the full Phase 1
journey at 1920x1080 and 100%, 125%, 150%, 175%, and 200% application scale.

## Equivalent-state assessment

### P15-01 — New airport overview

The implementation now reserves the center for the world and applies the same
rounded navy/cyan component family to top chrome, rails, cards, fields, action
trays, and footer controls. Airport creation is one contextual tray rather than
a column of proof instructions.

Highest-impact remaining differences:

1. the concept has a curated river/road/forest parcel while the baseline uses
   one seamless grass surface;
2. the concept uses bespoke pictogram artwork and an illustrated location card;
3. the implementation still uses the engine font rather than a final licensed
   project type family.

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

The offer state now receives one central rounded contextual card with identity
and reward evidence separated into inner surfaces. Pin, decline, and accept
actions use reusable quiet, destructive, and positive variants. Duplicated
compatibility prose was removed from the persistent rail.

Highest-impact remaining differences:

1. the concept has a full timetable grid rather than one exact five-minute
   recommendation;
2. compatibility is summarized in prose instead of a large checklist card;
3. the operator has no final crest or aircraft icon.

Intentional difference: Phase 1.5 does not add Phase 4 timetable mechanics or
new scheduling data solely to imitate the mockup.

### P15-04 — Aircraft turnaround

The approved Cessna source is present as a fictional yellow Riverbend aircraft
on Stand A1, with protected selection/service markers and a compact rounded
turnaround card. Inspection and fuel are expressed as two short, independent
status lines. The aircraft remains presentation-only and selects from 16
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
7. Reopened the phase after the owner rejected the first runtime boards as too
   primitive.
8. Centralized rounded surfaces, typography, fields, status chips, and six
   button variants in `AMSimUITheme`.
9. Rebuilt the production shell and development component gallery from those
   shared primitives, then removed persistent diagnostic and duplicated offer
   copy.
10. Added revision-triggered layout prepasses, true collapsed action rows, and
    dedicated medium-scale context geometry after 125% and 150% captures exposed
    clipping.
11. Repeated the five-scale visual sweep from the exact corrective candidate
    and generated four new equivalent-state comparison boards.

## Technical verification

- `AMSimEditor` builds on Unreal Engine 5.8.0.
- The project audit passes with the approved Phase 1.5 content review.
- The final automation report contains zero failed, not-run, or in-process
  tests and includes component-language coverage for rounded surfaces, outlines,
  distinct action variants, and disabled treatment.
- The deterministic rendered journey retains checksum
  `6583174326702355518`.
- The corrective 100%, 125%, 150%, 175%, and 200% captures pass with zero 8x
  backlog. Medium layouts show all contextual actions without clipping, and
  175-200% uses compact drawers.
- Manual PIE inspection confirmed a visible OS cursor, hover/click behavior,
  focus restoration, and progression through the representative states.
- A clean Shipping cook produces 267 packages with no missing presentation
  dependency.
- The production root Widget Blueprint and presentation assets are present in
  the cooked registry; the development component gallery is absent.
- The Shipping runtime remains alive through the five-second launch check.
- Forbidden Shipping file, text, and receipt matches are all zero.

## Acceptance record

The owner accepted the corrective component language and authorized Phase 2 on
2026-07-26. Phase 1.5 is complete. Later illustrated environment detail,
operator identity art, timetable grid depth, and service-worker animation
remain explicit follow-on scope rather than hidden Phase 1.5 claims.
