# Phase 6 Riverbend Longreach 787-9

This folder contains the single owner-approved external reference, the
project-owned editable vector master, deterministic raster exports, sixteen
reviewed headings, and the generated heading review board for Phase 6.

## Approval boundary

- External reference:
  `Boeing_787-9_source-approved.png`
- External reference SHA-256:
  `6AA56CD2CDD7035E3EE3526CFF5A603751E03441DA11ECE6C7E6E550B9B04449`
- Owner-approved treatment: **Riverbend Longreach — River & Sun**
- Approval date: 2026-07-28
- Approval does not cover any other file from the original source directory.

The external PNG is retained outside cooked content. Its real-operator color
treatment is not retained in the project-owned master.

## Production contract

- Editable master: `Riverbend_Longreach_787-9.svg`
- Runtime type: strict top-down Paper2D
- Real dimensions: 62.8 m length × 60.1 m wingspan
- Normalization: 32 pixels per meter
- Master orientation: nose right/east
- Runtime heading `H000`: nose up/north
- Pivot: center of the transparent raster canvas
- Livery: ivory fuselage, navy spine/tail, cyan river, amber sun
- Rendering: flat colors, no gradients, no 3D asset requirement

`CreatePhase6Aircraft.py` rasterizes the SVG, generates sixteen 22.5-degree
heading variants, writes checksums and dimensions to `aircraft-manifest.json`,
and creates `Riverbend_Longreach_787-9_heading-review.png`.

The automatic VGC trace under `../VGC/787-source-trace` is retained as
provenance. Its strict validation rejected the trace for insufficient visual
fidelity, so it is not a production master or runtime source.
