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
| Scalability | Phase 1 `Reference` profile: medium effects, high UI/sprite clarity |
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

## Development-host interpretation

Results from the Phase 0 CORSAIR VENGEANCE i5200, Intel Core Ultra 9 285K, 63.4 GiB RAM, and RTX 5090 are labeled `development host`. They can prove correctness, deterministic continuity, package boundaries, and that the software is not intrinsically frame-locked below budget. They cannot alone prove reference-tier performance.

## Close condition

Phase 1 performance closes only after the machine-readable result names a physical machine meeting or falling below this tier. Until then, performance status is `development-host passed; reference-tier pending`.
