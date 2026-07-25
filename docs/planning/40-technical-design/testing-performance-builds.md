# TS-08: Testing, Performance, Diagnostics, and Builds

**Status:** Approved baseline  
**Owner:** Quality engineering  
**Last updated:** 2026-07-25

## Outcome

Every implementation phase produces repeatable evidence that rules are correct, saves remain compatible, performance meets budget, content is valid, and the Windows package runs offline.

## Test tiers

1. `Unit`: fixed-step domain rules, formulas, IDs, serialization primitives, graph algorithms.
2. `Simulation`: headless scenarios driven by commands with event/checksum assertions.
3. `Integration`: subsystem lifecycle, content resolution, command/query adapters, async saves.
4. `Content`: data assets, registries, tags, maps, text, provenance, compatibility, path viability.
5. `UI`: view-model states, input actions, accessibility, build previews, alerts.
6. `Functional`: rendered vertical-slice journeys and editor workflows.
7. `Performance`: deterministic load fixtures, long soak, save latency, memory, route spikes.
8. `Package`: clean Windows build, launch, new/save/load/recover, offline/network-denied run.

Tests use stable fixtures and seeds. Flaky retries cannot convert a failure to success without recording the original failure and owner.

## Budgets

Reference targets at 1920 x 1080:

- 60 FPS presentation with GS-20 target load;
- simulation median under 4 ms/frame and 99th percentile under 8 ms/frame at 1x;
- total game-thread frame 99th percentile under 16.6 ms at 1x;
- 8x remains responsive and does not skip steps, with backlog warning if hardware cannot sustain it;
- pause/save snapshot capture under 50 ms on the game thread;
- autosave serialization/write target under 2 seconds on reference storage;
- memory target below 4 GB at maximum supported map and population;
- no unbounded growth during a 4-real-hour soak.

Hardware reference and scalability tier must be recorded before Phase 1 closes; budgets then become measured release gates.

## Diagnostics

Developer builds expose simulation step/backlog, entity counts by domain, proxy counts, route/cache statistics, reservation waits, command rejections, event rate, save state, random stream counters, and state checksum. Player diagnostics use friendly messages and avoid paths or technical details unless an export is explicitly requested.

Crash/diagnostic bundles are local and opt-in. They exclude passenger generated names where practical, save contents, credentials, and personal filesystem data.

## Build gates

Each phase requires compilation, automated suites for owned systems, content validation, save round-trip, deterministic replay, performance fixture, clean packaged launch, and updated traceability evidence. Shipping builds exclude editor modules, MCP, Python, RemoteControl, test-only commands, and debug data.

## Acceptance and source

The build pipeline must produce a fresh Windows package from a clean checkout, run the smoke suite without network, and emit machine-readable results. The release candidate cannot ship with test failures, broken content references, save migration failures, or unexplained performance budget regression.

Source: [Automation Test Framework](https://dev.epicgames.com/documentation/en-us/unreal-engine/automation-test-framework-in-unreal-engine).

