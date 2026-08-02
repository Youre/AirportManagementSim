# Phase 1 Movement-Surface Provenance

**Status:** Reviewed runtime source kit
**Owner:** Presentation engineering and visual design
**Generated:** 2026-08-02
**Generator:** Local ComfyUI, Flux 2 Klein text-to-image workflow
**References:** VA-02, CT-01, VC-BLD-08

## Purpose and boundary

These project-generated fictional surface textures make the starter runway and
taxiway recognizable at management zoom without changing their simulation
capability. They contain no real airport identity, operator branding, copied
game artwork, readable text, or 3D runtime content.

ComfyUI supplied unmarked aggregate material candidates only. Exact runway
edge lines, center dashes, threshold bars, and taxiway yellow guidance were
drawn deterministically by
`scripts/ui/New-Phase1MovementSurfaceSources.ps1`. This avoids AI-generated
marking errors and keeps the output reproducible.

## Generation record

Service: `http://192.168.5.12:8188`
Workflow: `flux_2_klein_t2im.json`
Model: `flux-2-klein-4b.safetensors`
Dimensions: 1024x1024
Negative prompt: empty
Variants reviewed: four runway and four taxiway candidates

Runway prompt:

> A perfectly orthographic square material study of a compacted charcoal-gray
> fine aggregate airfield runway surface, filling the entire frame edge to
> edge. Subtle small stone grain, restrained weathering, faint rubber wear and
> repaired tonal variation, uniform scale, no directional lighting. No paint,
> no lines, no symbols, no objects, no borders, no curbs, no perspective, no
> horizon, no text. Seamless tileable 2D texture for a premium friendly airport
> management game, readable but quiet at management zoom.

Retained request ID: `fe11c8c2-f7da-4168-9305-22a4552ba940`
Seed: `3157424076374297827`

Taxiway prompt:

> A perfectly orthographic square material study of a slightly warm dark-gray
> compacted fine aggregate taxiway surface, filling the entire frame edge to
> edge. Fine granular texture, subtle wheel-path wear and gentle tonal
> variation, uniform scale, no directional lighting. No paint, no lines, no
> symbols, no objects, no borders, no curbs, no perspective, no horizon, no
> text. Seamless tileable 2D texture for a premium friendly airport management
> game, readable but quiet at management zoom.

Retained request ID: `c2703591-9fb9-4213-9078-a7573e80332a`
Seed: `3157424076374297831`

## Retained files

| File | Dimensions | SHA-256 | Decision |
| --- | --- | --- | --- |
| `SourceAssets/Phase1.5/Surfaces/Generated/runway-aggregate-comfyui-master.png` | 1024x1024 | `DFF2AFC1C6C0368A6CCF5665B9D45716168FDC766B4CB7E396A6D8C4BA8A425F` | Retained unmodified source master |
| `SourceAssets/Phase1.5/Surfaces/Generated/taxiway-aggregate-comfyui-master.png` | 1024x1024 | `A3485F984B0039B0ACD98CB68C429548AB034B3925E55719D56C4978D756E4E0` | Retained unmodified source master |
| `SourceAssets/Phase1.5/Surfaces/T_RunwayMarked.png` | 512x512 | `91A1A87F046F6F3E8E67A756D9ECBDA8F418086B7B832002B28E7D076D7771AF` | Approved derived runway source |
| `SourceAssets/Phase1.5/Surfaces/T_TaxiwayMarked.png` | 512x512 | `80708E9FB7ED22C0E4D4426C4D5F75046A349C83F2E6DD41426BC4986D239553` | Approved derived taxiway source |

## Processing and review

The deterministic script center-crops each source, palette-normalizes it,
constructs mirrored 256-pixel quadrants for exact 512-pixel edge continuity,
and applies project-authored markings. Two-by-two review sheets verify that the
aggregate has no visible edge seam. Final review confirms:

- strict top-down material presentation;
- no baked objects, text, borders, perspective, or directional light;
- quiet grain that remains legible without competing with workers or aircraft;
- off-white runway markings and amber taxiway markings consistent with VA-02;
- color-independent runway/taxiway distinction through marking geometry;
- service roads and unfinished earthwork continue using the separate unmarked
  taxi-wear source.

Unreal destinations:

- `/Game/Phase1/Presentation/Textures/Surfaces/T_RunwayMarked`
- `/Game/Phase1/Presentation/Sprites/Surfaces/S_RunwayMarked`
- `/Game/Phase1/Presentation/Textures/Surfaces/T_TaxiwayMarked`
- `/Game/Phase1/Presentation/Sprites/Surfaces/S_TaxiwayMarked`

The runtime presenter holds constructor-time hard references. The clean
Development package built on 2026-08-02 contains both textures, both sprites,
and both texture bulk-data entries. Its rendered Phase 1 journey exercised the
finished surfaces and passed state, save/load, screenshot, and offline checks.
