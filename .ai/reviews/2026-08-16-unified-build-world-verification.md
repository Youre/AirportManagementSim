# Unified build-world verification

Date: 2026-08-16
Result: Passed

## Outcome

Build mode is now an interaction layer over the same Paper2D airport world used
by normal play. The construction view no longer renders substitute facilities
or owns a second map coordinate system.

## Redundancy retired

- Removed the UMG runway, taxiway, service-road, terminal, gate, parcel-grid,
  and world-handle artwork from `UAMSimConstructionProposalView`.
- Removed build-view loads of the superseded regional-terminal and stand
  textures.
- Removed the rule that hid authoritative Phase 1 world geometry while build
  mode was open.
- Replaced the widget-local map rectangle conversion with Slate-to-viewport
  deprojection, airport-plane intersection, and the shared inverse
  `MapPhase1WorldToPoint` conversion.
- Reduced the touched implementation by 510 net lines: 573 additions (including
  the new focused preview module) and 1,083 deletions before documentation.

## Retained boundaries

- `AAMSimWorldPresenter` owns committed infrastructure, fixed starter context,
  the schema-10 terminal, and the bounded transient preview pool.
- `UAMSimConstructionProposalView` owns tools, validation, pointer gestures,
  instructions, cancel, and confirmation.
- Phase 1 simulation remains authoritative for validity, spending,
  construction, persistence, and worker behavior. No schema or gameplay rule
  changed.

## Automated and build proof

- Focused Unreal automation: 3/3 passed.
  - `AMSim.Phase1_5.Presentation.Paper2DWorld`
  - `AMSim.Phase4_5.Presentation.ConstructionValidationContent`
  - `AMSim.TerminalGrowth.Presentation.RevisionGatingAndProxyReuse`
- Full Unreal automation: 81/81 passed with zero failed, incomplete, or
  in-process results.
- UE 5.8 `AMSimEditor Win64 Development`: passed.
- UE 5.8 `AMSim Win64 Development`: passed.
- Clean Development BuildCookRun/archive: passed in 99.93 seconds.
- Clean Shipping BuildCookRun/stage/package: passed in 74.94 seconds.
- Packaged Development integrated smoke: passed, including journey assertions,
  save/load continuity, state assertions, and screenshot capture.
- Shipping package: launched windowed, remained responsive for more than eight
  seconds, and closed after the check.
- Release-boundary scan: zero runtime string loads, forbidden runtime
  dependencies, forbidden Shipping filenames/text matches, or 2,000-line
  source violations; one valid Shipping IoStore catalog found.
- The complete schema-10 Phase 7 release audit passed after retiring its stale
  schema-9 expectation.

## Interaction proof

A real 1280x720 Unreal replay verified:

- the generated schema-10 terminal roof and both starter gates remain in the
  same location when build mode opens;
- no duplicate UMG airport artwork appears;
- runway and taxiway gestures render directly in the world with the normal
  movement-surface assets;
- a runway-to-Gate-A taxi connection reaches `NETWORK READY`;
- confirmation retains exact geometry and replaces the preview treatment with
  the construction treatment;
- preview markers clear after confirmation and the mouse remains visible.

Right-click panning remains delegated to the existing root camera path while
build mode is open. Automated camera coverage and source review passed; the
desktop input harness used for this replay could not inject a right-button drag,
so owner feel-testing remains the final subjective confirmation.

## Package and cache record

- Canonical launcher:
  `D:\AMSim-Current\Windows\AMSim.exe`
- Canonical Development inner binary SHA-256:
  `D3ACFCA2573585DC04BABD08562266146F7477B20BE3E8E8B1A565567FD1029C`
- Shipping staged binary SHA-256:
  `59D1C50A483B7D8F713CB95B9D5D42DEB116A4A7216AF7F908FF3F2291CB8D1D`
- All 22 canonical save files (17,296,295 bytes) were restored with zero hash
  mismatches after the clean archive.
- Cache measurement stayed healthy: project caches decreased from 8.82 GiB to
  8.58 GiB, shared Unreal caches remained 0.8 GiB, C retained 130.0 GiB free,
  and D retained 696.5 GiB free. No shared cache was cleared.
- A 17 MB temporary save safety copy remains at
  `C:\Users\dave\AppData\Local\Temp\AMSim-Current-SaveBackup-20260816`
  because environment policy blocked its deletion after restoration.

## Remaining owner check

Use the canonical executable to judge right-click pan feel and continue the
consolidated schema-10 acceptance journey. No separate build-world renderer or
alternate executable is involved.
