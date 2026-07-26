# Phase 1.5 Presentation Sources

This directory holds reviewed source masters and deterministic derived PNGs. Unreal imports cooked assets from these sources; the game never reads this directory at runtime.

- `Aircraft/` contains the single owner-approved Cessna 152 source and 16 normalized heading derivatives.
- `Surfaces/` contains palette-normalized seamless derivatives of reviewed local ComfyUI candidates.
- `Props/` contains project-authored deterministic 2D props and utility sprites.

Generation is reproducible with `scripts/ui/New-Phase15PresentationSources.ps1`. Provenance and the aircraft reuse decision are recorded in the Phase 1.5 planning and import-manifest documents.
