# Compact HUD Density Adversarial Review

Date: 2026-08-02
Status: Approved with controls below

## Failure modes and controls

1. **Icons become ambiguous.** Reuse one reviewed Riverbend icon family and
   preserve matching hover/focus labels, tooltips, and accessible text.
2. **Hover expansion shifts or shrinks the world.** Paint flyouts in an overlay
   above the map; never change the rail or map layout width on hover.
3. **Flyouts block map input after the pointer leaves.** Keep the flyout
   hit-test-inert and collapse it from both unhover and focus removal.
4. **Small icons reduce clickability.** Keep every icon button at least
   88-by-56 units with visible hover/focus treatment.
5. **Disabled tools look active or play success audio.** Preserve enabled state
   on the underlying themed button and do not route click outcomes from disabled
   controls.
6. **Construction loses operational meaning.** Always show the explicit stage,
   runway designators, and cost; retain warnings and remedies in their existing
   evidence surfaces when a blocker exists.
7. **The compact card still covers the work.** Anchor it in the upper-right map
   corner, clear of the rail and central work focus, and prohibit nested
   evidence boxes.
8. **High UI scale truncates the label or card.** Allow flyout/card text to wrap
   at 175-200% without reducing font size; verify the responsive fixture.
9. **Build mode becomes inconsistent.** The construction editor continues to
   collapse normal gameplay chrome through the existing visibility boundary.
10. **Cooked builds lose icons.** Use constructor-time UObject references to
    the existing explicitly cooked Phase 4.5 UI kit and include icon-load
    assertions in focused coverage.

## Required proof

Compile editor and game targets; verify hover/focus expansion, stable collapsed
geometry, concise construction-card mapping, icon loading, and disabled state;
run full automation and the integrated rendered smoke; compare the identical
1920x1080 construction state with VA-01 and VA-02; inspect cursor, world area,
and text-independent status; run line-limit and diff checks.
