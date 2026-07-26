# TS-01: Runtime Architecture and Module Boundaries

**Status:** Approved baseline  
**Owner:** Runtime architecture  
**Last updated:** 2026-07-25

## Outcome

The Unreal project separates deterministic simulation, game orchestration, presentation, editor tooling, and tests so gameplay remains inspectable, testable, and independent of visual Actors.

## Module layout

| Module | Type | Responsibility | May depend on |
| --- | --- | --- | --- |
| `AMSim` | Runtime | Executable bootstrap and game modes | Gameplay, UI |
| `AMSimSimulation` | Runtime | Fixed-step state, commands, domain rules, queries, snapshots | Core, CoreUObject; minimal engine utilities |
| `AMSimGameplay` | Runtime | Unreal lifecycle, content resolution, world services, orchestration | Simulation, Engine |
| `AMSimUI` | Runtime | Paper 2D presentation, UMG/CommonUI, input-facing adapters, audio | Gameplay |
| `AMSimEditor` | Editor | Import, validation, authoring, MCP-safe toolsets | Runtime modules, editor APIs |
| `AMSimTests` | Developer | Unit, integration, content, save, performance fixtures | All testable runtime modules |

The existing `AMSim` blank module is the bootstrap seed, not the final home for every system. Dependency direction cannot point from simulation to UI, editor, widgets, Actors, or audio.

## Service lifetimes

`UAMSimGameInstanceSubsystem` owns profile settings, content catalog, save catalog, save/load coordination, and transition into an airport. `UAMSimAirportSimulationSubsystem` owns one active airport simulation, fixed-step clock, command queue, event stream, and query publication. Editor-only validation and import services live in `AMSimEditor`.

Subsystem lifetimes are explicit and appropriate because Unreal subsystems are automatically instanced classes tied to engine/game-instance/world lifetimes. No global mutable singleton may bypass these lifetimes.

## C++ and Blueprint boundary

C++ owns authoritative simulation rules, identifiers, serialization, routing, economy formulas, command validation, events, queries, performance-critical presentation pooling, and automated tests. Blueprint/UMG owns widget composition, visual sequences, content wiring, authored presentation, and designer-adjustable data assets.

Blueprint can request a typed command and render a query snapshot. It cannot directly mutate authoritative records, compute canonical prices/outcomes, or store state that must survive saving. Blueprint events triggered by visuals are cosmetic unless they return through a validated command.

## Runtime flow

1. Input produces an intent.
2. UI translates intent to a typed command.
3. The world subsystem validates and queues it.
4. At a simulation boundary, the command changes state or returns a structured rejection.
5. State emits domain events and publishes an immutable query snapshot.
6. UI, audio, and pooled presentation proxies consume those outputs.
7. Save coordination captures a snapshot at a safe boundary.

## Plugin baseline

Paper2D and CommonUI are explicit runtime dependencies. Enhanced Input remains enabled. TextToSpeech is optional behind CT-03's provider. DataRegistry may support lookup. MassEntity and StateTree are not the authoritative simulation foundation; adoption requires measured benefit and architecture review. ModelContextProtocol, Python, RemoteControl, and toolsets are editor/developer-only and excluded from shipping.

## Acceptance and sources

Architecture tests must prove the simulation module runs without a rendered world, UI cannot mutate records directly, subsystem creation/destruction preserves ownership, and shipping builds exclude editor/MCP dependencies.

Sources: [Programming Subsystems](https://dev.epicgames.com/documentation/en-us/unreal-engine/programming-subsystems-in-unreal-engine), [Paper 2D overview](https://dev.epicgames.com/documentation/unreal-engine/paper-2d-overview-in-unreal-engine?lang=en-US), [CommonUI](https://dev.epicgames.com/documentation/unreal-engine/common-ui-plugin-for-advanced-user-interfaces-in-unreal-engine?lang=en-US).
