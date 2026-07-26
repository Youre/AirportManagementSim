# Phase 2 Visual Review

**Status:** Passed for the Phase 2 in-scope presentation
**Reviewed:** 2026-07-26
**References:** VA-05, VA-06, VA-07

## Evidence set

The integrated rendered fixture captured the same deterministic airport state at
1920 x 1080 and 100%, 125%, 150%, 175%, and 200% application scale. Primary
100% comparisons are stored in the generated evidence directory:

- `AMSim/Saved/Phase2/Comparisons/VA-05-Phase2.png`;
- `AMSim/Saved/Phase2/Comparisons/VA-06-Phase2.png`;
- `AMSim/Saved/Phase2/Comparisons/VA-07-Phase2.png`.

The complete scale matrix is under `AMSim/Saved/Phase1/Scale/<percent>/`. These
generated files are verification artifacts rather than source assets.

## Shared visual-language result

The Phase 2 surfaces retain the accepted Phase 1.5 component language:

- deep navy framing with cyan outlines and amber emphasis;
- rounded cards, status surfaces, and buttons with consistent state treatment;
- strong header, objective, contextual status, world, and control regions;
- concise operational copy with cause and next-action text;
- a world-dominant normal layout and compact drawers at 175% and 200%;
- readable disabled, destructive, warning, and active states without color-only
  meaning.

The first Phase 2 capture exposed overlapping detail copy in the contextual
rail. The bounded iteration shortened repeated labels and moved detailed truth
into concise status lines. The recaptured states have no overlapping actions or
operational text at the supported scales.

## VA-05: turnaround

The implementation preserves the concept's aircraft-operation hierarchy:
current operation first, service state second, and compatible actions last.
Aircraft and vehicle state comes from the immutable Phase 2 query and is also
represented by pooled Paper2D proxies.

Highest-impact remaining differences:

1. The concept uses a large aircraft-focused inspection surface; the Phase 2
   baseline keeps turnaround inside the persistent world and contextual rail.
2. The current world proxies use deterministic editable silhouettes rather
   than the concept's illustrative ground-service art.
3. Individual task dependency details are summarized in the rail instead of
   remaining open as a large service checklist.

These are intentional at this gate. Phase 2 proves concurrent service,
exclusive resources, and direct dispatch without replacing the world-dominant
management layout. A later selected-aircraft inspector may expand the same
query without changing simulation ownership.

## VA-06: weather and incident

The implementation matches the reference intent with a persistent condition
summary, a red color-independent incident cause, explicit `NEXT` guidance,
response action, construction/closure state, and a visible world incident
overlay.

Highest-impact remaining differences:

1. Weather is expressed through category text and a bounded world tint rather
   than the reference's dramatic storm illustration.
2. The disabled-aircraft marker is deliberately schematic and smaller than the
   concept incident focal point.
3. The three-day forecast is available to the query and fixture but is
   summarized rather than displayed as a full-width strip.

The differences are intentional for a non-catastrophic Phase 2 incident. The
player-facing cause, remedy, response, containment, reopening, cost, and rating
effects remain explicit.

## VA-07: specialization and progression

The Phase 2 implementation exposes its three in-scope identities—general
aviation, flight school, and charter—together with current objective, contract
state, rating, economy, expansion, and recovery actions.

Highest-impact remaining differences:

1. VA-07 is a future all-path capability map; the Phase 2 surface intentionally
   omits cargo, passenger, and mixed-airport paths owned by later phases.
2. The three current identities use compact reusable action variants rather
   than large facility-thumbnail cards.
3. Progress evidence is condensed into the contextual rail rather than a
   full-screen tree.

This is an intentional staged implementation of VA-07. Phase 2 establishes the
three living-GA branches and reusable status/action language; later phases may
compose those components into the complete capability map without inventing a
second theme.

## Acceptance

The in-scope Phase 2 presentation passes because every Phase 2 mutation is
available without debug controls, primary states remain readable at every
supported scale, compact transitions preserve actions, the mouse/input policy
is unchanged, and the visual differences above are scoped and explained.
