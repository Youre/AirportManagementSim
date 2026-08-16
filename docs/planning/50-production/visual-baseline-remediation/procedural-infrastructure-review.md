# Procedural Infrastructure Review

**References:** VA-01 airport overview, VA-02 construction planning

**Equivalent state:** packaged `VisualBaseline`, 1920x1080, 100% UI scale

**Before:** [sprite-based overview](audit/17-packaged-final-visual-baseline-overview.png)

**After:** [procedural overview](audit/18-packaged-procedural-infrastructure-overview.png)

## Decision

Runways, taxiways, service roads, aprons, and gate pads use dimension-driven
flat procedural geometry as their authoritative presentation. Raster sprites
remain appropriate for aircraft, buildings, props, terrain detail, and
deliberate interaction markers, but they no longer determine movement-surface
length, width, junction shape, or placement bounds.

The same geometry builders serve placement preview, construction progress,
completed Phase 1 infrastructure, and the mature-airport overview. This keeps
the build and operations views spatially identical and prevents another
parallel renderer from developing.

## Equivalent-state findings

### Corrected

- Long surfaces are continuous instead of repeated or scale-stretched tiles.
- Runway, taxiway, road, apron, and gate dimensions now follow world-space
  measurements at every zoom level.
- Shared height and translucency-sort layers keep movement surfaces below
  terminals, buildings, aircraft, vehicles, vegetation, gates, and interaction
  markers.
- Taxiways and roads have scalable shoulders and markings. Aprons have inset
  borders and panel seams. Gate pads have parking guidance, stop bars, and
  color-independent safety envelopes.
- Existing build-mode endpoint, snap, connection, crossing, and blocked-node
  cues remain independent sprites above the procedural preview surface.

### Highest-impact differences found and bounded correction

1. **The first procedural apron was a pale, undifferentiated slab.** It now has
   a distinct neutral palette, inset perimeter, deterministic panel grid, and
   warmer individual gate pads.
2. **The first packaged pass lacked surface hierarchy.** Runway, taxiway, road,
   apron, and gate values are now intentionally separated while markings keep
   the VA-01 white/amber operational language.
3. **Plain centerlines looked more schematic than the concept.** Scalable edge
   shoulders, dashed road markings, threshold bars, parking lines, stop bars,
   and safety envelopes restore operational detail without raster variants.

## Intentional differences and continuing polish

- VA-01 uses denser environmental dressing, organic curved access roads,
  weathering, runway numbers, signs, edge lights, and more localized markings.
  These remain presentation-detail work; they should be added as procedural or
  deterministic overlay layers without replacing the authoritative geometry.
- The mature validation fixture intentionally exposes the full infrastructure
  network for sorting and continuity review. A later world-composition pass may
  make some service links less visually dominant while retaining their exact
  topology.
- The bottom aircraft inspector and category-specific activity drawers remain
  separate VA-01 remediation priorities.

## Verification

- Focused procedural geometry, Paper2D-world, preview pooling, sorting, and
  terminal-coexistence automation passes.
- Complete Unreal automation passes: 82 succeeded, zero failed.
- Clean Development and Shipping BuildCookRun passes with 748 cooked packages.
- Packaged Development journey and Shipping launch checks pass offline.
- Release audit reports zero runtime string asset loads, forbidden
  editor/MCP/test dependencies, required 3D assets, or source line-limit
  violations.
