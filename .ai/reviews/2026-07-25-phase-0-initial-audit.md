# Phase 0 Initial Audit

**Status:** Complete
**Date:** 2026-07-25
**Scope:** Replacement-workstation migration closeout and Phase 0 Unreal baseline

## User-visible outcome and non-goals

Outcome: the portable `AMSim` scaffold is associated with Unreal Engine 5.8, uses an explicit 2D/offline project baseline, and points planning/provenance controls at the replacement workstation's aircraft source.

Non-goals:

- no gameplay systems;
- no aircraft import or rights approval;
- no Phase 1 content;
- no investigation of the historical source-machine CPU failures or the driver-install GPU interruption.

## Architecture-impact review

Affected ownership boundaries:

- `AMSim.uproject`: engine association and explicit plugin boundary;
- `DefaultEngine.ini`: renderer, RHI, hardware profile, startup map, and network-related defaults;
- `AMSim.Build.cs`: Paper 2D, CommonUI/UMG, and input-facing module dependencies;
- TS-01 and its planning review: correct obsolete `APSim*` names to authoritative `AMSim*` names;
- CT-01 and planning context: relocate the read-only aircraft source without granting import approval.

Public save, command, event, query, and gameplay schemas do not change. The deterministic simulation/presentation separation, strict 2D direction, offline runtime, and import-manifest gate remain invariant.

Migration is configuration-only because gameplay implementation and shipped saves do not exist. Rollback is source-control reversion of the baseline files. Verification requires JSON/config inspection, a clean editor-target compile, Unreal startup evidence, documentation link/UTF-8 checks, and a source-asset reconciliation.

## Audit observations

- Unreal Engine `5.8.0`, changelist `55116800`, is installed at `C:\Program Files\Epic Games\UE_5.8`.
- Audit host: Corsair Vengeance i5200, Windows 11 Pro build 26200, Intel Core Ultra 9 285K, 63.4 GB RAM, NVIDIA GeForce RTX 5090 driver `32.0.16.1074`.
- The editor associated `AMSim.uproject` with `5.8`.
- The blank project retained maximum-quality 3D renderer settings and an editor-generated Android File Server configuration that is outside the offline baseline.
- The authoritative project/module prefix is `AMSim`; remaining `APSim*` planning names are stale.
- The replacement aircraft source is `C:\Users\dave\Documents\Joes_Game\dist\assets`.
- That directory contains 45 PNG aircraft files matching the inventory's recorded dimensions. Deployed filenames carry build-hash suffixes.
- The inventory's Airbus A330-300 SVG record is not present in the replacement source and remains unavailable until separately restored and reviewed.

## Adversarial review

Checks for hidden behavior, migration, security, operability, and stale context found:

- Switching from DX12/SM6 maximum-quality defaults to DX11/SM5 scalable defaults is intentional for the initial 2D proof baseline. A later measured need can revise the RHI through architecture review.
- Disabling ray tracing, Lumen-style global illumination/reflections, mesh distance fields, virtual shadows, skin-cache shaders, Substrate, exposure, bloom, lens flare, and motion blur removes blank-template assumptions; it does not change authoritative simulation behavior.
- The tracked Android File Server security token and network-enabled settings were removed. Android File Server and online subsystem plugins are explicitly disabled at project scope.
- Unreal Editor itself mounts several engine-default editor/online support plugins even with the project-level runtime plugins disabled. This startup audit is not proof of the TS-09 Shipping boundary; the Phase 0 packaged dependency and network-denied audits remain required.
- The baseline uses `/Engine/Maps/Entry` because no project-owned Phase 0 proof map exists yet. Creating that orthographic proof map is follow-up work.
- The headless startup used NullRHI and therefore proves project/plugin/config initialization, not rendered DX11 performance. The rendered 60 FPS proof remains part of the Phase 0 gate.
- No save or content migration exists. Generated binaries, caches, logs, and IDE files remain ignored and are reproducible.
- Hashed deployed aircraft filenames can change when `Joes_Game` is rebuilt. Exact source filename plus checksum is therefore mandatory in the future import manifest.

## Verification

- `AMSimEditor Win64 Development` compiled successfully with Unreal Engine 5.8.0 using MSVC 14.44 and Windows SDK 10.0.26100.0.
- Headless Unreal startup initialized the engine, loaded Paper 2D and CommonUI, opened `/Engine/Maps/Entry`, reported zero map errors/warnings, and exited with code 0.
- All 45 available aircraft PNGs matched the inventory dimensions; only `Airbus_A330-300.svg` was unavailable.
- Planning text is valid UTF-8, local Markdown links resolve, obsolete planning references are absent, required/disabled plugin assertions pass, renderer assertions pass, and `git diff --check` passes.
