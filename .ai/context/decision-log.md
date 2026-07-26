<!-- ai-project-init:begin managed -->
# Decision Log

- Initial AI engineering OS scaffold generated.
<!-- ai-project-init:end managed -->

## Project decisions

- 2026-07-25: Unreal Engine replaces the Godot reference in `ideas.txt` as the target engine.
- 2026-07-25: The game is 2D, cartoon-styled, and top-down; 3D assets and 3D presentation are excluded.
- 2026-07-25: `AirplaneGame` is a read-only source of candidate 2D aircraft art until an import and rights review is approved.
- 2026-07-25: The concept-definition-only stage is superseded by the approved implementation-planning set rooted at `docs/planning/00-foundation/specification-index.md`.
- 2026-07-25: Runtime simulation uses deterministic lightweight C++ records with stable IDs; Actors, sprites, widgets, and audio are presentation only.
- 2026-07-25: The rendering baseline is an orthographic Paper 2D world with CommonUI/UMG; the product remains 2D-only.
- 2026-07-25: Saves use versioned asynchronous snapshots, migrations, atomic replacement, and previous-known-good recovery.
- 2026-07-25: The initial product is offline single-player Windows desktop with mouse and keyboard; it has no gameplay backend, account, analytics, cloud, or multiplayer dependency.
- 2026-07-25: Unreal MCP and related automation are localhost editor-only tools and are excluded from Shipping builds.
- 2026-07-25: All six airport specializations are viable endgames; passenger international-hub growth is optional rather than the universal success path.
- 2026-07-25: The seven-image concept-art set under `docs/planning/30-content-and-assets/concept-art/` is approved supporting evidence for visual comparison gates. Written specifications remain authoritative; generated values, geometry, typography, dimensional shading, and incidental details are non-normative.
