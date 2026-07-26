# Phase 1 Reference Hardware and Scalability Tier

**Status:** Approved reference target; physical-tier validation required for phase close
**Owner:** Quality engineering
**Last updated:** 2026-07-26

## Purpose

This document defines the representative Windows tier used to interpret TS-08 Phase 1 budgets. The CORSAIR/RTX 5090 development workstation remains useful for deterministic, build, and upper-bound presentation evidence, but it is not the representative target.

## Reference tier

| Component | Phase 1 reference target |
| --- | --- |
| Operating system | Windows 11 64-bit |
| CPU | 6 physical cores, 12 threads, 3.0 GHz sustained or better |
| Memory | 16 GiB system RAM |
| GPU | DirectX 11/SM5 discrete GPU with 4 GiB dedicated VRAM |
| Storage | SSD with at least 10 GiB free for package, saves, and caches |
| Display | 1920 x 1080 at 60 Hz |
| Scalability | Phase 1 `Reference` profile defined below |
| Input | Mouse and keyboard |
| Network | Not required; inbound and outbound denial supported |

This is a capability tier rather than a single vendor SKU. Physical validation records the actual CPU, GPU, driver, RAM, storage, OS build, package hash, and scalability values used.

## Budgets

- presentation: 60 FPS, p99 game-thread frame at or below 16.6 ms;
- simulation at 1x: median below 4 ms and p99 below 8 ms;
- 8x: no skipped authoritative steps; backlog remains bounded and visible;
- snapshot capture on the game thread: below 50 ms;
- save serialization/write on reference SSD: below 2 seconds;
- resident memory: below 4 GiB;
- no unbounded growth during the Phase 1 soak.

## Reference scalability profile

The packaged evidence runner applies and records these Unreal scalability-group
values. The profile minimizes 3D-only work while retaining high-quality sprite
sampling and antialiasing:

| Group | Value |
| --- | ---: |
| View distance | 0 |
| Antialiasing | 2 |
| Shadows | 0 |
| Global illumination | 0 |
| Reflections | 0 |
| Post processing | 0 |
| Textures | 2 |
| Effects | 1 |
| Foliage | 0 |
| Shading | 1 |

UI scale remains independently verified at 100%, 125%, 150%, 175%, and 200%.

## Physical measurement

Copy or build the final Development and Shipping packages on the candidate
machine, review the actual hardware against the capability tier, and run:

```powershell
.\scripts\phase1\Test-Phase1ReferenceTier.ps1 `
    -TierAttestation AtOrBelowApprovedTier
```

The default run executes the packaged S01 journey, then measures a four-real-hour
1x soak. It records the source commit when available, exact package hashes,
Windows build, CPU, GPU/driver, RAM, display, package drive, applied scalability
values, frame and simulation percentiles, save timings, backlog, process memory,
and periodic memory samples.

Formal acceptance also requires a clean Git worktree so the recorded commit,
package hashes, and source state are unambiguous.
Before launching, the collector requires all four Development/Shipping binaries
to match `scripts/phase1/Phase1AcceptanceManifest.json`; a rebuilt or modified
package cannot reuse the recorded package identity.

The soak passes when maximum process memory remains below 4 GiB and the
last-quarter memory-sample median is no more than 64 MiB above the first-quarter
median. This threshold tolerates normal allocator/cache settling while rejecting
sustained material growth in the bounded Phase 1 fixture.

`-AllowIncompleteEvidence -SoakSeconds 5` is only a harness check. It cannot
close the reference-tier gate.

## Development-host interpretation

Results from the Phase 0 CORSAIR VENGEANCE i5200, Intel Core Ultra 9 285K, 63.4 GiB RAM, and RTX 5090 are labeled `development host`. They can prove correctness, deterministic continuity, package boundaries, and that the software is not intrinsically frame-locked below budget. They cannot alone prove reference-tier performance.

## Close condition

Phase 1 performance closes only after
`AMSim/Saved/Phase1/reference-tier-result.json` names a physical machine meeting
or falling below this tier and reports `passed: true`. Until then, performance
status is `development-host passed; reference-tier pending`.

When the reference-tier and tester records both pass, run
`.\scripts\phase1\Test-Phase1Acceptance.ps1`. The aggregate result must report
`passed: true` before the Phase 1 planning status changes to complete.
