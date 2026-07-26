# Phase 0 Architecture-Impact Review

**Status:** Approved for implementation
**Date:** 2026-07-25
**Scope:** PI-01 Phase 0, TS-01 through TS-09 and CT-01

## Outcome and non-goals

Phase 0 produces an empty-airport technical foundation that runs deterministically without presentation, exposes inspectable command/event/query boundaries, saves and recovers asynchronously, renders a minimal orthographic 2D proof, validates static definitions and asset provenance, and builds/runs offline on Windows.

It does not implement the Phase 1 grass-airfield gameplay journey, copy aircraft art, approve rights, add accounts/backends, or ship 3D gameplay assets.

## Modules and ownership

- `AMSim`: executable bootstrap only.
- `AMSimSimulation`: engine-independent fixed-step state, IDs, random streams, commands, events, queries, checksums, spatial records, snapshots.
- `AMSimGameplay`: Unreal lifecycle, content resolution, save coordination, and world orchestration.
- `AMSimUI`: orthographic Paper 2D presentation, CommonUI shell, input adapters, and presentation proxies.
- `AMSimEditor`: editor-only audit/import-manifest validation commands.
- `AMSimTests`: developer automation tests spanning the Phase 0 contracts.

Dependency direction is `Simulation <- Gameplay <- UI <- AMSim`, with Editor and Tests depending inward. Simulation cannot depend on Actors, Widgets, Paper 2D, CommonUI, editor APIs, networking, or wall-clock outcomes.

## Public contracts

- save-stable opaque 64-bit entity and command IDs;
- fixed 250-game-millisecond clock;
- deterministic named random streams and stable iteration;
- typed command envelope with structured validation;
- immutable sequenced events;
- revisioned query snapshots;
- versioned empty-airport snapshot with checksum;
- integer-centimeter spatial coordinates, one-meter cells, and 64-meter chunks;
- versioned static definition/provenance records;
- presentation proxies keyed only by stable entity ID.

## Invariants

The implementation preserves strict 2D presentation, offline single-player operation, deterministic authoritative state, independent saves, editor-only automation, no aircraft import before manifest approval, and the absence of authoritative state in Actors or Widgets.

## Migration and rollback

There are no shipped saves or gameplay assets. Schema version starts at 1; every later authoritative representation change requires a migration fixture. Rollback is source reversion plus removal of generated ignored state. Save writes use temporary files and previous-known-good backup rotation so rollback never overwrites the only valid snapshot.

## Required evidence

- editor and game targets compile;
- automation proves clock, random stream, replay checksum, command validation, spatial addressing, save round-trip/backup, presentation proxy identity, and manifest validation;
- two identical empty-airport command streams produce identical checksums/events;
- save/load continuation matches uninterrupted execution;
- orthographic Entry proof launches at 1920 x 1080 and records frame evidence;
- clean Windows package launches with network denied;
- package/config audit excludes editor, MCP, Python, RemoteControl, Android File Server, and project online dependencies;
- CT-01 manifest template exists and no source sprite is copied.

## Review roles

Architecture/simulation, save/test, UI/accessibility, platform/security, and content/provenance review are required. Adversarial review runs after implementation and before Phase 0 is declared complete.
