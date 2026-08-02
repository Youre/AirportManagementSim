# Phase 1.5 Presentation Sources

This directory holds reviewed source masters and deterministic derived PNGs. Unreal imports cooked assets from these sources; the game never reads this directory at runtime.

- `Aircraft/` contains the single owner-approved Cessna 152 source and 16 normalized heading derivatives.
- `Surfaces/` contains palette-normalized seamless derivatives of reviewed local ComfyUI candidates. The marked runway and taxiway retain their unmodified aggregate masters under `Surfaces/Generated/` and are reproduced by `scripts/ui/New-Phase1MovementSurfaceSources.ps1`.
- `Props/` contains project-authored deterministic 2D props and utility sprites.

Generation is reproducible with `scripts/ui/New-Phase15PresentationSources.ps1`. Provenance and the aircraft reuse decision are recorded in the Phase 1.5 planning and import-manifest documents.

The movement-surface generation record is
`docs/planning/30-content-and-assets/phase-1-movement-surface-provenance.md`.
