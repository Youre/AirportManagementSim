<!-- ai-project-init:begin managed -->
# Current State

Project: AirportManagementSim
Material profile: documents
Initialization mode: augment-existing

Active priorities:
- Initialize durable AI-native engineering context.

Current architectural tensions:
- None recorded yet.

Next verification command:
- `ai-init doctor .`

Last verification:
- None recorded yet. Use `ai-init record-verification` after meaningful checks.
<!-- ai-project-init:end managed -->

## Project-specific state

Active priorities:

- Migrate the repository to a stable replacement Windows workstation using `.ai/handoffs/2026-07-25-windows-migration-resume.md`.
- Review the approved specification set beginning at `docs/planning/00-foundation/specification-index.md`.
- Use `docs/planning/30-content-and-assets/concept-art/README.md` for mapped visual comparison gates.
- When implementation is authorized, open Phase 0 in `docs/planning/50-production/phased-implementation-roadmap.md`.
- Preserve the 2D-only Unreal direction and complete the approved import manifest before copying aircraft sprites.

Current tensions:

- The original i9-14900K workstation has confirmed processor-core WHEA errors and is not a valid Unreal build/verification host.
- Real-aircraft naming, performance data, and livery rights require later review.
- The experimental Unreal TextToSpeech and MCP plugins require isolation and fallback; neither may become a shipped dependency.
- Performance budgets need a recorded reference-PC specification during Phase 0.

Verification note:

- Repository inspection and the initial 46-file aircraft inventory were completed on 2026-07-25.
- The implementation planning baseline selects Unreal Engine 5.8, orthographic Paper 2D, deterministic lightweight simulation records, versioned async saves, Windows mouse/keyboard, and a fully offline product.
- Seven reviewed concept-art references now cover overview, build mode, terminal flow, timetable planning, turnaround, weather/incidents, and specialization progression.
- Portable Unreal source is under `AMSim/`; `APSim` is obsolete.
- The next session must check out `codex/planning-docs-concept-art`, install/associate Unreal 5.8, and regenerate all ignored build state locally.
- Gameplay implementation has not started.
- Until `ai-init` is installed on `PATH`, run the CLI module with the skill source on `PYTHONPATH`.
