# Phase 1.5 Architecture-Impact Review

**Status:** Approved for implementation  
**Date:** 2026-07-26  
**Change:** Visual foundation, reusable HUD regions, and Paper2D world presentation

## Outcome and non-goals

Phase 1.5 replaces the proof-map and debug-density HUD with an approved visual system while preserving every Phase 1 simulation, command, save, economy, and scheduling contract. It does not add Phase 2 gameplay, authoritative Actor state, runtime MCP, or 3D presentation.

## Boundaries

- `AMSimSimulation` remains authoritative and unchanged.
- `AMSimGameplay` continues to expose immutable `FPhase1QuerySnapshot` values and typed commands.
- `AMSimUI` gains a presentation-only view state and a Paper2D presenter. Neither may retain raw simulation pointers or write simulation records.
- `UAMSimRootScreen` remains the command adapter. A native Widget Blueprint subclass owns the persistent HUD regions; native code populates and updates those regions.
- `AAMSimWorldPresenter` owns visual components and receives snapshots from the root screen. It has no per-frame tick and cannot complete gameplay work.

## Assets and cooking

- The required root Widget Blueprint and PaperSprite assets use constructor-time or serialized hard references.
- Generated textures and the approved Cessna 152 source are imported into a bounded Phase 1 presentation folder.
- Source masters, generation prompts, checksums, import settings, and review decisions remain outside cooked content.
- Shipping audits continue to reject editor, MCP, Python, RemoteControl, tests, and toolset dependencies.

## Compatibility and rollback

- Save schema and deterministic checksums are unchanged because presentation state is not serialized.
- The native fallback HUD remains available if the Widget Blueprint cannot load, but fallback activation cannot satisfy package acceptance.
- Rollback consists of restoring the previous HUD class and GameMode presenter spawn; no save migration is required.

## Verification obligations

- View-state mapping, revision gating, presenter visibility/sorting, and query-only behavior receive focused automation.
- Four deterministic visual states are captured at every supported UI scale.
- Development and Shipping packages must exercise the intended Widget Blueprint and Paper2D assets.

