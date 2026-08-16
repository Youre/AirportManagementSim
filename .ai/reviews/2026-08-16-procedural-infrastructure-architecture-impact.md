# Procedural Infrastructure Architecture Impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

- Runways, taxiways, service roads, and gate/apron footprints retain precise
  width, clean edges, consistent markings, and readable intersections at every
  supported camera zoom.
- Build previews, active construction, and completed facilities no longer swap
  between stretched raster treatments.
- Existing aircraft, vehicles, buildings, terminal interiors, people, props,
  and landscape art remain unchanged.

## Technical boundary

- Add one runtime-only flat procedural-surface component backed by Unreal's
  `ProceduralMeshComponent` plugin. It generates XY-plane triangles with no
  collision and uses a cooker-visible unlit vertex-color material.
- `AAMSimWorldPresenter` remains the only airport-world renderer. It owns the
  procedural components and consumes the same Phase 1 proposal/query geometry
  used by simulation and interaction.
- Geometry builders are pure/testable and contain no simulation mutation,
  asset loading, command dispatch, or camera logic.
- Build preview, construction progress, overlay modes, and completed state call
  the same strip, disc/junction, rounded-rectangle, and marking builders with
  different colors and visibility.
- Existing save schemas and simulation records remain unchanged. No generated
  mesh or presentation state is serialized.

## Rendering contract

- Base surfaces use generated polygons. Markings use separate generated strips
  and polygons above their parent surface.
- Taxiway joins receive generated node discs so connected segments read as one
  network. Taxiway/runway crossings preserve the runway base while carrying the
  taxi centerline through the intersection.
- Runway designation text remains query-derived; thresholds, center dashes, and
  edge bands are generated relative to actual length and width.
- Gate/apron geometry is a rounded pad with lead-in line, stop bar, safety
  envelope, and terminal-facing connection edge. Decorative podiums, doors,
  vehicles, and aircraft remain sprites above it.
- World-presentation height and sort layers remain authoritative. Roads stay
  below runways/taxiways and gate/apron surfaces; markings stay below buildings
  and interaction markers.

## Content and cooking

- Enable `ProceduralMeshComponent` explicitly and add its runtime module to
  `AMSimUI`.
- Create `/Game/Phase1/Presentation/Materials/M_ProceduralInfrastructure`
  through a reproducible editor script. The material renders vertex color as
  unlit emissive color with vertex alpha controlling opacity.
- Load the material through a constructor-time hard reference. No runtime
  string asset loads, editor module dependency, authored Static Mesh, or
  required 3D asset is introduced.

## Compatibility and rollback

- The former raster surface sprites remain cooked only until all retained
  mature/incident presentation consumers are audited; Phase 1 stops using them
  as authoritative geometry.
- Rollback is presentation-only: restore the prior surface component types and
  keep the unchanged proposal/save contracts.
- Existing pointer snapping, graph validation, construction timing, aircraft
  routing, economy, and persistence must remain byte-for-byte behaviorally
  compatible.

## Required proof

- Pure geometry tests for segment width, caps, rounded pads, junctions,
  markings, and deterministic mesh output.
- Presenter tests for one authoritative procedural surface family, layer
  ordering, construction progress, overlay colors, and preview/completed
  geometry equivalence.
- Existing build-mode interaction, Phase 1 construction, aircraft journey,
  save/load, and full automation remain green.
- Equivalent-state VA-01 and VA-02 comparison boards plus real packaged
  inspection at 1920 x 1080 and a compact window.
- Clean Development package, cooker/material inventory, release dependency
  scan, and offline packaged smoke.
