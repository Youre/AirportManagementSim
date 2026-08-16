# VA-02 Build-World Recovery Visual Comparison

- Reference: `docs/planning/30-content-and-assets/concept-art/02-build-mode-runway-extension.png`
- Reported regression: `before-user-report.png`
- Corrected capture: `after-connected-network.png`
- Road/gate sorting stress capture: `road-gate-sorting-stress.png`
- Comparison board: `va02-comparison.png`
- Equivalent state: construction planning with a proposed runway, a valid taxi connection, parcel cues, and network readiness.

## Review result

- [x] The world remains the dominant surface, with HUD chrome confined to rails and compact status regions.
- [x] Primary, secondary, and contextual actions have a clear hierarchy.
- [x] The Riverbend cyan, navy, coral, and warm-success palette forms one language across world overlays and chrome.
- [x] Proposed surfaces use an outline plus diagonal pattern rather than color alone.
- [x] Distinct connection, connected, crossing, and blocked symbols remain legible above facilities.
- [x] The parcel grid is subordinate to the world and the proposed network.
- [x] The mouse remains visible throughout the captured interaction.
- [x] The road/gate stress capture confirms that movement surfaces render below both starter gate footprints while interaction nodes remain above them.

## Highest-impact remaining differences

1. VA-02 depicts an established, dense airport; the implemented state is deliberately the empty starter-airfield journey and therefore has much less operational scenery.
2. VA-02 has a thumbnail-driven construction catalog; the starter slice still exposes only the three supported freeform tools as text buttons.
3. VA-02's right inspector uses richer visual metrics and surface thumbnails; the implementation keeps the currently supported cost, validity, connection, and readiness facts in a compact text card.

## Intentional differences

- The reference is a runway-extension state, while this capture validates first-airfield placement. Later expansion work can reuse the same planning-grid, pattern, node, and layer rules without introducing another renderer.
- Runway and taxiway placement remain freeform pointer gestures rather than choosing fixed segment thumbnails.
- Only facilities present in the starter gameplay contract are shown; concept-only mature-airport scenery was not fabricated for this verification.

## Bounded correction completed

- Recovered the parcel grid and boundary, source-size-correct proposal geometry, translucent plan fills, diagonal construction patterns, and explicit connection-state nodes.
- Centralized world height and sort priorities so terrain, construction beds, roads, runways, taxiways, gates, structures, plan patterns, and interaction markers have a durable ordering contract.
- Verified the corrected connected-network state and a deliberately overlapping service-road/gate state in the live Unreal renderer.

## Continuing visual debt

- Target: richer starter-facility scenery and an icon-based build catalog as the construction inventory grows.
- Expected visual change: more of VA-02's operational density without increasing panel clutter or reducing the world viewport.
- Acceptance observation: future build surfaces reuse these overlay and layer primitives, with no alternate UMG geometry path.
