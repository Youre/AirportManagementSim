# Phase 1 Verification Record

**Status:** Implementation verification passed; external phase gate pending
**Date:** 2026-07-26
**Engine/platform:** Unreal Engine 5.8.0, Win64
**Scenario:** `S01.StarterGrassAirfield`
**Map:** `Map.TemperateStarter`
**Seed:** `11684030530412666515`

## Verified implementation

- deterministic starter construction, delivery, building, inspection, opening, cancellation/refund, closure/reopen, and recovery;
- immediate paused compatible offer with pin/decline/accept contracts;
- exact five-minute schedule, explicit Stand A1 assignment, protected stand buffers, persistent airframe, automated runway/taxi/service/departure state, five captioned radio intents, ledger, rating, objective, and reward;
- editable airport name and independent local slot, save/load controls, query-backed status/remedy surfaces, local speech abstraction, and caption/cue fallback;
- snapshot schema 2, pure schema-1 migration, boundary continuation matrix, deterministic replay/checksum, async verified writes, backup fallback, and packaged continuation;
- project-owned map, 20 validated Primary Assets, internally authored 2D aircraft marker, and zero copied external aircraft files.

## Automated results

| Gate | Result |
| --- | --- |
| Editor build | Passed |
| Project/content audit | Passed; 20 required Primary Assets resolved |
| Full automation | 24/24 passed; 14 Phase 0 plus 10 Phase 1 |
| Schema/replay/save | Passed |
| UI scale matrix | Passed at 100%, 125%, 150%, 175%, and 200% |
| Development package | Built, launched, and completed packaged S01 |
| Shipping package | Built and clean-launched for five seconds |
| Shipping dependency scan | 0 forbidden file, text, or receipt matches |
| 2D content boundary | Only `.uasset`/`.umap`; 0 required 3D candidates |
| Socket observation | 0 unexpected Development; 0 Shipping |

The machine-readable aggregate is generated at `AMSim/Saved/Phase1/pipeline-result.json`. Scale captures are generated under `AMSim/Saved/Phase1/Scale/<percent>/`; final packages are generated under `AMSim/Saved/Phase1Packages/`.

## Final development-host package measurement

| Metric | Result |
| --- | ---: |
| Rendered resolution | 1920 x 1080 |
| Frames / elapsed | 6,203 / 5.056168 s |
| Average FPS | 1,226.819 |
| p99 frame | 1.172 ms |
| Maximum frame | 163.851 ms |
| Snapshot capture | 0.023 ms |
| Save write | 8.157 ms |
| Maximum 8x backlog | 0 steps |
| Resident memory | 441 MiB |
| Final economy | 2,200 Credits / 5 Airport Points |
| Phrase intents | 5 |
| Final checksum | `8264913351739008826` |

This is RTX 5090 development-host evidence, not representative-tier certification.

## UI-scale evidence

| Scale | Result | p99 frame | Layout |
| ---: | --- | ---: | --- |
| 100% | Passed | 1.535 ms | three rails; local scroll |
| 125% | Passed | 1.164 ms | three rails; local scroll |
| 150% | Passed | 1.237 ms | three rails; local scroll |
| 175% | Passed | 1.145 ms | stacked full-width scroll |
| 200% | Passed | 1.284 ms | stacked full-width scroll |

Visual inspection confirmed no objective/status or ledger/rating collision after the final fixes. Scrollbars intentionally preserve access where all controls cannot fit simultaneously.

## Visual-reference comparison

- VA-01: retains a clear airport/status/economy hierarchy and north-up parcel; implementation uses flat project-owned 2D geometry rather than incidental concept-art detail.
- VA-02: exposes cost, project stage, cause/remedy, affected facilities, and visible construction state; proposal geometry is the deterministic starter bundle.
- VA-04: exposes compatibility, pin/decline/accept, exact slot, stand, buffer, and departure details; only the Phase 1 one-flight timetable is shown.
- VA-05: exposes persistent airframe identity, movement state, inspection, fueling, captions, and completion feedback; service animation remains derived.

Written 2D, accessibility, value, and behavior requirements take precedence over generated reference dimensions and ornamental detail.

## External phase-close evidence still required

1. Run `scripts/phase1/Test-Phase1NetworkDenied.ps1` from elevated PowerShell against the final packages.
2. Run the same packaged scenario on a physical machine meeting or falling below the approved reference tier and record hardware/scalability measurements.
3. Record an unassisted new-tester completion/comprehension review, including audible local speech and caption/local-cue fallback.

Until those records pass, Phase 1 implementation is verified but the formal phase gate remains open.
