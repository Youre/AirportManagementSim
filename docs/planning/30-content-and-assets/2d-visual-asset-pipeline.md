# CT-01: 2D Visual Direction and Asset Pipeline

**Status:** Approved baseline  
**Owner:** Visual content  
**Last updated:** 2026-07-25

## Outcome and scope

The game presents a coherent cartoon airport entirely through 2D artwork in an orthographic top-down world. This specification owns visual scale, sprite directions, layers, animation, import requirements, source provenance, and the review gate for the existing AirplaneGame assets.

## Non-negotiable direction

- Runtime world presentation is 2D. Do not introduce 3D aircraft, buildings, vehicles, terrain, characters, or effects as gameplay assets.
- Unreal uses an orthographic camera and Paper 2D sprites/flipbooks. Simple engine geometry may support collision or editor visualization only when invisible in the shipped presentation.
- UI uses 2D vector/raster assets through UMG/CommonUI.
- The world must remain readable at overview and close operational zooms, with shape and icon differences that do not depend on color alone.

## Concept-art baseline

The [Concept Art Reference Set](concept-art/README.md) defines the approved visual direction for world/UI balance, palette, readable top-down silhouettes, overlay treatment, and operational tone. VA-01 is the primary overview reference; VA-03 and VA-05 establish interior and close-operations density; VA-06 establishes poor-weather readability.

These images are directional rather than production assets. Their dimensional shading must be translated into 2D sprite treatment, and their incidental numbers, geometry, aircraft shapes, and text are non-normative.

## Coordinate and scale contract

Simulation space is meters. Art uses a project-wide pixels-per-meter value stored in import configuration; the initial baseline is 32 pixels per meter for world assets. Aircraft source art is normalized to real type length and wingspan rather than displayed at arbitrary sprite-file size. Oversized interactive outlines and selection markers may improve usability without changing collision or clearance dimensions.

Sprites use a common top-down orientation with forward direction recorded in metadata. Required aircraft directions are generated or authored at 16 headings; smooth rotation is allowed only when the art passes quality review. Buildings and fixed equipment may use four orientations. Sorting layers are terrain, markings, below-vehicle effects, structures, people/bags, vehicles/aircraft, above-vehicle effects, and selection/UI.

## Source asset intake

The inventory in `../90-research/aircraft-asset-inventory.csv` records 46 top-down source files: 23 catalogued civilian/commercial candidates, 14 catalogued military exclusions, and 9 uncatalogued candidates requiring identification. It does not grant reuse approval.

Before any file is copied from `C:\Users\Dave\Documents\AirplaneGame`, an import manifest must record:

- source path and checksum;
- creator/license and reuse decision;
- depicted aircraft and accuracy confidence;
- transparency, dimensions, orientation, and duplicate status;
- required cleanup, scale, palette, directional variants, and livery treatment;
- destination asset ID and reviewer.

Only approved civilian candidates enter the core catalog. Military art remains outside core progression unless a later event-specific decision approves it. The parallel source folders must not be imported as duplicates.

## Animation and effects

Animations are short readable state cues: propeller/rotor motion, beacon/navigation lights, vehicle wheels where visible, passenger walking, belt motion, doors, construction work, weather, and emergency lights. Particle-like visuals remain sprite-based. Damage uses restrained status variants, smoke, barriers, and icons; no graphic human harm.

## Import and validation

Import presets enforce texture group, nearest/bilinear filtering by art style, no unintended mip bleed, transparent padding, pivot, pixels per unit, naming, atlas policy, and collision metadata. Source masters remain separate from cooked Unreal assets. Derived variants must be reproducible through documented editor automation.

Automated validation rejects missing provenance, unsupported dimensions, duplicate checksums, incorrect pivots, missing direction metadata, real operator branding, military core tags, and absent accessibility silhouette checks. Visual review covers scale, readability, clipping, rotation, night/weather treatment, and 2,000-visible-entity load.

## Acceptance

The pipeline is accepted when one aircraft, person, service vehicle, building, marking, belt, and weather effect can move from approved source to a packaged 2D scene reproducibly; when no 3D asset is required; and when the aircraft-import manifest is approved before any cross-repository copy occurs.
