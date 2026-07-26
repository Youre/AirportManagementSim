# Phase 0 Adversarial Review

**Status:** Passed with documented residuals
**Date:** 2026-07-25 local / 2026-07-26 UTC
**Scope:** PI-01 Phase 0 closeout

## Review question

Could the foundation appear to pass while violating determinism, save safety, 2D/offline boundaries, package isolation, provenance controls, or operability?

## Findings resolved before closeout

### Editor commandlet discovery

The audit commandlet was initially unavailable because `AMSimEditor` loaded at `PostEngineInit`. The module now loads at `Default`, allowing Unreal to discover the commandlet without introducing the module into game or Shipping targets.

### False-positive proof capture

The initial screenshot was black even though the process launched. The CommonUI root lifecycle, `CommonGameViewportClient`, and screenshot UI capture were corrected. The pipeline now rejects a black capture and the final proof visibly identifies the empty-airport Phase 0 state.

### Shipping dependency breadth

Engine-default plugins caused an early staged package to include broad editor/plugin metadata. The project now disables engine plugins by default and opts in only to required runtime capabilities. Shipping scans show no AMSim editor/test modules, MCP, Python, RemoteControl, or ToolsetRegistry files, receipt references, or staged-manifest text matches.

### Incorrect child-process observation

The first launcher monitoring path could observe the bootstrap process instead of the packaged child executable. The scripts now resolve and monitor the real packaged executable. This exposed Unreal's expected developer trace-control listener in Development and confirmed that Shipping exposes zero TCP sockets.

### Snapshot mutation and appended-data acceptance

Restore validation could previously mutate state before all input was known valid, and snapshot decoding did not reject appended bytes. Restore now validates before mutation and enforces aligned clock/revision values, sorted unique bounded IDs, magic, CRC, and exact body length. Corruption and continuation tests pass.

### Generated-artifact disk pressure

Repeated package output exhausted the system drive and interrupted Zen-backed cooking. Only reproducible generated outputs were moved to `D:\AMSimPhase0Generated-20260726`; no source or user asset moved. Zen was restarted and the complete final pipeline was rerun successfully to `D:\AMSimPhase0Final-20260726`.

### Firewall elevation and cleanup

Network denial cannot be proven by creating firewall rules without elevation. A dedicated elevated script now creates exact per-executable inbound and outbound block rules and removes them in `finally`. The elevated run passed, and no temporary rules remained.

## Boundary and regression checks

- Authoritative simulation state remains in lightweight records rather than Actors or Widgets.
- Named random streams, stable iteration, replay, snapshot continuation, and checksums are automated.
- Save replacement is asynchronous, verifies temporary output before rotation, retains a previous-known-good backup, and falls back after corruption.
- Slot writes are serialized/coalesced per slot.
- Static definitions reject duplicate IDs, missing dependencies, cycles, and invalid tag roots.
- Spatial state uses integer centimeters, one-meter cells, 64-meter chunks, base occupancy layers, deterministic reservation order, and per-chunk revisions.
- The runtime is 2D-first with an orthographic camera and no required 3D gameplay asset.
- No candidate aircraft image was imported.
- Shipping has no observed TCP socket and succeeds with network denied.
- Generated artifacts, saves, logs, binaries, packages, and IDE state remain untracked.

## Accepted residuals

### Transitive editor descriptor metadata

Three editor-only plugin descriptor rows remain in staged metadata through required CommonUI/EnhancedInput transitive references: `DataValidation`, `EngineAssetDefinitions`, and `GameplayTagsEditor`. No corresponding editor module or binary is staged, none belongs to the prohibited MCP/Python/test set, and Shipping receipt/file scans are clean. This is accepted as descriptor metadata rather than a shipped editor runtime.

### Engine Entry map

The proof still uses `/Engine/Maps/Entry`. It now renders the project-owned CommonUI proof shell, but Phase 1 should replace the engine map with a project-owned grass-airfield map.

### Performance host representativeness

The Phase 0 measurement host is high-end and proves the 60 FPS gate on that host only. It is not a minimum or release-reference tier. Selecting and testing a representative reference PC remains due before Phase 1 closes.

### Development trace listener

Development exposes Unreal's local trace-control listener on port 1985. The firewall-denied journey passes and Shipping exposes zero sockets. Release evidence must continue to test Shipping, not infer its boundary from Development.

### Save schema migration fixture

Schema 1 has no older shipped schema to migrate. This is correct for the first foundation schema. The first authoritative schema change must add a migration fixture before its phase can close.

## Rollback and follow-up

Phase 0 is source-only and has no shipped user migration. Rollback is a source-control reversion plus deletion of reproducible ignored output. Phase 1 must preserve the stable simulation/save boundaries, introduce a project-owned map, and make reference-hardware selection explicit before its gate closes.

No unresolved finding blocks Phase 0.
