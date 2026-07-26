<!-- ai-project-init:begin managed -->
# Architecture Invariants

- Preserve public contracts unless a migration plan exists.
- Keep ownership boundaries explicit.
- Prefer local patterns before introducing new abstractions.
- Document cross-cutting changes before implementation.
<!-- ai-project-init:end managed -->

## Game-specific invariants

- Unreal Engine is the target engine.
- The shipped presentation is 2D and top-down; do not introduce 3D assets or a 3D-dependent design.
- Unreal MCP is a development and inspection tool, never a shipped runtime dependency.
- Treat `C:\Users\Dave\Documents\AirplaneGame` as a read-only source library until an asset-import plan is approved.
- Keep simulation rules separate from presentation so aircraft, economy, weather, and progression remain data-driven and testable.
