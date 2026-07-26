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
| Full automation | 25/25 passed; 14 Phase 0 plus 11 Phase 1; machine-readable report has 0 failed/not-run/in-process |
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
| Frames / elapsed | 6,074 / 5.000395 s |
| Average FPS | 1,214.704 |
| p99 frame | 1.271 ms |
| Maximum frame | 320.003 ms |
| 1x simulation median / p99 | 0.000 / 0.001 ms per frame |
| Snapshot capture | 0.028 ms |
| Save write | 8.652 ms |
| Maximum 8x backlog | 0 steps |
| Resident memory | 432 MiB ending / 447 MiB maximum |
| Final economy | 2,200 Credits / 5 Airport Points |
| Phrase intents | 5 |
| Final checksum | `8264913351739008826` |

This is RTX 5090 development-host evidence, not representative-tier certification.

The run applied the documented Phase 1 `Reference` scalability profile: view
distance 0, antialiasing 2, shadows/global illumination/reflections/post process
0, textures 2, effects 1, foliage 0, and shading 1.

## UI-scale evidence

| Scale | Result | p99 frame | Layout |
| ---: | --- | ---: | --- |
| 100% | Passed | 1.268 ms | three rails; local scroll |
| 125% | Passed | 1.316 ms | three rails; local scroll |
| 150% | Passed | 1.381 ms | three rails; local scroll |
| 175% | Passed | 1.491 ms | stacked full-width scroll |
| 200% | Passed | 1.512 ms | stacked full-width scroll |

Visual inspection confirmed no objective/status or ledger/rating collision after the final fixes. Scrollbars intentionally preserve access where all controls cannot fit simultaneously.

## Visual-reference comparison

- VA-01: retains a clear airport/status/economy hierarchy and north-up parcel; implementation uses flat project-owned 2D geometry rather than incidental concept-art detail.
- VA-02: exposes cost, project stage, cause/remedy, affected facilities, and visible construction state; proposal geometry is the deterministic starter bundle.
- VA-04: exposes compatibility, pin/decline/accept, exact slot, stand, buffer, and departure details; only the Phase 1 one-flight timetable is shown.
- VA-05: exposes persistent airframe identity, movement state, inspection, fueling, captions, and completion feedback; service animation remains derived.

Written 2D, accessibility, value, and behavior requirements take precedence over generated reference dimensions and ornamental detail.

## Final package identity

| Package file | SHA-256 |
| --- | --- |
| Development launcher | `6E313C72D7EAF9147AEBE8B3ADB593FAC4D75C6EEEEA818A201E677A222EC39F` |
| Development runtime | `5A67AD0EDEFD31893B5AE5BFED6D6A5B4ED1A4060BA196B3885621220DFF3A5E` |
| Shipping launcher | `DEF5538AC925382C0F5FFE98FE9689C66EA921595043D72D6BE9A2C321DE474B` |
| Shipping runtime | `73F229B63E43EF6E9857E4B7C6DD932D5B8919C584FCE00D9C7922AF0215F920` |

## Closeout hardening evidence

- The first closeout pipeline attempt passed build, audit, automation, and all UI
  scales, then failed when AutomationTool lost its local Zen oplog stream during
  Development staging. The failure is preserved rather than relabeled.
- A retry exposed a four-of-five phrase-intent result at 100% scale. Flight
  catch-up now processes every intermediate state in order; the
  `AMSim.Phase1.Movement.CatchUpPreservesIntermediateEffects` regression forces a
  full scheduled-to-completed catch-up and verifies services, reward, and all five
  phrases.
- The pipeline now parses Unreal's exported automation `index.json`; process exit
  code 0 alone can no longer hide failed, not-run, or in-process tests.
- The one success-with-warning is the intentional Phase 0 backup fixture deleting
  the current snapshot before verifying backup recovery.
- `Test-Phase1ReferenceTier.ps1` passed its five-second collector check and
  correctly reported formal acceptance false because this host is unreviewed and
  the four-hour soak was not requested.

## External phase-close evidence still required

1. Run `scripts/phase1/Test-Phase1NetworkDenied.ps1` from elevated PowerShell against the final packages.
2. Run `scripts/phase1/Test-Phase1ReferenceTier.ps1 -TierAttestation AtOrBelowApprovedTier`
   on a physical machine meeting or falling below the approved reference tier.
3. Complete the
   [unassisted new-tester protocol](../../docs/planning/50-production/phase-1-new-tester-protocol.md),
   including audible local speech and caption/local-cue fallback.

Until those records pass, Phase 1 implementation is verified but the formal phase gate remains open.
