# Procedural Infrastructure Preliminary Adversarial Review

Date: 2026-08-16
Status: Approved with controls below

## Failure cases and controls

1. **Procedural rendering becomes a hidden 3D-art dependency.** Generate only
   flat XY-plane triangles in C++; add no Static Mesh, skeletal asset,
   perspective camera, lighting requirement, or collision.
2. **The new component becomes a second world renderer.** Replace Phase 1
   surface sprites in the existing presenter. Do not add another actor, canvas,
   coordinate mapping, or preview-only pathway.
3. **Preview and completed geometry diverge.** Route proposal, construction,
   and completed states through the same pure geometry builders and compare
   their footprints in automation.
4. **Roads again cover gates.** Enforce both height and translucency sort
   priority against the shared layer contract and retain an explicit regression
   assertion.
5. **Overlapping taxi segments show seams.** Use a world-consistent flat color
   base and generated node junctions; avoid segment-local baked texture
   gradients at joins.
6. **Runway markings stretch or become nonsensical on short geometry.** Place
   threshold, edge, number, and dash geometry from usable runway length and
   clamp dash count/spacing.
7. **Construction flickers between placeholder and final assets.** Keep one
   surface component and update its progress/color; the construction bed
   remains separately below it and never alternates ownership.
8. **Dynamic mesh rebuilding causes frame spikes.** Rebuild only when the
   revision/proposal/progress bucket changes; batch each surface family and
   disable collision, navigation, shadows, and ticking.
9. **Translucent ordering becomes unstable.** Use explicit height separation
   and translucency priorities from `AMSimWorldPresentationLayers`; opaque
   completed surfaces should not rely on component creation order.
10. **A material is missing in Shipping.** Create it reproducibly, load it via
    constructor-time hard reference, explicitly cook `/Game/Phase1`, and fail
    presentation automation if the material or procedural component is absent.
11. **Overlay colors overwrite base style permanently.** Store presentation
    state as derived colors and regenerate it from the current overlay and
    lifecycle state rather than mutating simulation or accumulated mesh data.
12. **Existing saves change appearance unpredictably.** Derive geometry solely
    from preserved endpoints, widths, gates, and construction progress. Run
    schema 1-10 migrations and the reusable `VisualBaseline` journey.
13. **Procedural gates erase terminal interaction affordances.** Preserve gate
    anchor positions and render lead-in, stop, safety, and connection markings;
    terminal doors and operational props remain above the pad.
14. **The change exceeds source-organization limits.** Put the component and
    pure mesh builder in focused files; do not expand the 2,000-line root
    presenter.

## Acceptance boundary

The rollout passes only when roads, taxiways, runway, gate/apron pads, previews,
and construction surfaces visibly use the shared procedural system in the
packaged build. A road-only proof is insufficient for completion.
