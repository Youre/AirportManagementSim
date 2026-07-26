# Phase 1 Implementation Architecture-Impact Review

**Status:** Passed
**Date:** 2026-07-26
**Scope:** Implemented PI-03 grass-airfield vertical slice

## Outcome

The implemented slice preserves the approved six-module direction and adds no new runtime service boundary:

- `AMSimSimulation` owns Phase 1 state, commands, validation, deterministic progression, events, queries, checksums, and schema-2 persistence;
- `AMSimGameplay` owns fixed-step Unreal lifecycle, save-slot orchestration, Primary Data Asset catalog resolution, and rendered/package smoke control;
- `AMSimUI` owns query-backed CommonUI/UMG presentation, semantic buttons/time controls, captions, and the replaceable local speech provider;
- `AMSimEditor` owns the project/content audit;
- `AMSimTests` remains developer-only and absent from Shipping;
- `AMSim` remains bootstrap.

No Actor, Widget, sound, screenshot, or presentation proxy authorizes a project stage, facility state, offer, reservation, service, transaction, reward, or progression result.

## Persistence impact

Snapshot schema 2 serializes the Phase 1 airport, construction, facility, team, offer, airframe, flight, stand-buffer, reservation, economy, rating, objective, phrase, and event records. Schema-1 migration initializes an unstarted Phase 1 domain without inventing completion.

Save/load continuation is exercised at pre-construction, construction, ready/open, inbound, taxi, turnaround, outbound, reward, pinned-offer, and packaged-journey boundaries. Strong-ID uniqueness, count bounds, schedule/buffer ordering, future/corrupt input, backup fallback, and deterministic continuation remain validation concerns rather than UI assumptions.

## Runtime and performance impact

The world subsystem advances authoritative state in fixed 250 ms steps with 1x/2x/4x/8x control and a visible no-skip backlog policy. The one-aircraft presentation uses derived 2D UMG geometry and one transient marker; there is no per-entity authoritative Actor Tick or 3D gameplay asset.

The final development-host packaged run recorded 1.172 ms p99 frame time, 0.023 ms snapshot capture, 8.157 ms save write, 441 MiB resident memory, and zero 8x backlog steps. These results demonstrate headroom but do not replace physical reference-tier validation.

## Content and dependency impact

The project-owned temperate map and 20 required Primary Assets resolve through validated types. The player-facing aircraft is an internally authored 2D marker under stable ID `Aircraft.LightPiston.Starter`; no file from `C:\Users\dave\Documents\Joes_Game\dist\assets` was copied.

The local TextToSpeech plugin is runtime-enabled behind `IAMSimSpeechProvider`. Captions remain authoritative and a local procedural cue is the failure path. Shipping scans found no editor, tests, MCP, Python, RemoteControl, or Toolset dependency and no required 3D asset candidate.

## UI and accessibility impact

The player can edit the airport name and independent save slot, inspect cause/remedy guidance, commit/cancel the starter project, open/close the airfield, pin/decline/accept the offer, schedule explicit Stand A1 ownership, inspect protected buffers/services/ledger/history/rating, control time, and save/load.

At 100–150% UI scale, each rail scrolls independently. At 175–200%, the page switches to one full-width stacked scroll surface. Visual inspection caught and resolved dynamic objective overlap and an overlong airframe-history collision before final packaging.

## Result

No implementation architecture issue blocks external Phase 1 acceptance. Remaining phase locks are evidence-environment constraints: elevated firewall-denied S15, physical reference-tier measurement, and unassisted tester/audio/comprehension review.
