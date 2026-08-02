# Compact HUD Density Verification

**Date:** 2026-08-02
**Scope:** Left map-navigation rail and active construction summary only

## Outcome

Passed. Normal map play now uses a compact icon-only left rail. Hover or
keyboard focus reveals a rounded text flyout without changing the rail width or
moving the world. The active construction summary is a two-line map-corner card
derived from authoritative Phase 1 query state.

## Automated verification

- Unreal Engine 5.8 Development Game target compiled successfully.
- Complete `AMSim` automation passed 74 of 74 tests with zero failures.
- `AMSim.Phase1_5.Presentation.CompactHud` passed. It verifies:
  - all eight navigation textures are available through constructor-time hard
    references;
  - hover and unhover delegates expand and collapse the flyout;
  - the tooltip and visible label carry the same action name;
  - disabling activation does not disable the explanatory wrapper or flyout;
  - the construction mapper emits the exact concise stage, runway, and cost
    strings and does not repeat starter-airfield, taxi-network, or gate prose.
- The integrated 1920 x 1080 rendered smoke passed journey, state assertions,
  save/load continuity, screenshot capture, reference-profile application, and
  performance sampling. It recorded 912.223 average FPS, 1.531 ms p99 frame
  time, zero 8x backlog, and checksum `13478235162240538523`.
- Source organization scan found zero C++ headers or sources over 2,000 lines;
  `AMSimRootScreen.cpp` is 1,955 lines after navigation extraction.
- `git diff --check` passed.

## Interaction and rendered proof

- Normal state: `AMSim/Saved/VisualReview/compact-hud-normal.png`.
- Forced hover state: `AMSim/Saved/VisualReview/compact-hud-hover.png`.
- The forced proof uses the production widget's expanded state after its actual
  hover/unhover delegate path passed automation. It shows `BUILD` beside the
  icon while the map, rail, activity card, and objective rail retain identical
  positions.
- Direct Windows pointer replay was attempted, but the local Development editor
  process was not exposed as a targetable Computer Use window. This record does
  not claim a manual pointer replay. The automated input-event path and rendered
  expanded-state proof cover the bounded behavior; owner feel remains available
  in the next ordinary gameplay session.

## Concept-art comparison

- VA-01 board: `AMSim/Saved/VisualReview/VA-01-compact-hud.png` with worksheet
  `.ai/reviews/2026-08-02-compact-hud-va01-comparison.md`.
- VA-02 board: `AMSim/Saved/VisualReview/VA-02-compact-construction-card.png`
  with worksheet
  `.ai/reviews/2026-08-02-compact-hud-va02-comparison.md`.
- The bounded density and hierarchy correction is accepted. Richer icon
  illustration, mature-airport density, construction props, and the remaining
  objective/footer density stay recorded as continuing visual debt rather than
  being misrepresented as a concept-art match.

## Cook and dependency assessment

No schema, simulation, persistence, balance, or runtime asset was added. The
rail reuses the already reviewed and cooked Phase 4.5 UI texture set through
constructor-time `FObjectFinder` references; no runtime string asset loading was
introduced. The change therefore uses the presentation-only package exemption
and does not produce another clean package. Existing clean-package evidence for
the reused atlas remains applicable.
