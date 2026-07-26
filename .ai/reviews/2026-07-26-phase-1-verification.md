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
| Elevated network-denied S15 | Passed; Development journey/save-load, expected local trace listener only, 0 Shipping TCP sockets, temporary rules removed |

The machine-readable aggregate is generated at `AMSim/Saved/Phase1/pipeline-result.json`;
elevated S15 is generated at `AMSim/Saved/Phase1/network-denied-result.json`.
Scale captures are generated under `AMSim/Saved/Phase1/Scale/<percent>/`; final
packages are generated under `AMSim/Saved/Phase1Packages/`.

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

## Elevated network-denied S15

At `2026-07-26T07:22:54.5810261Z`, Windows Firewall applied temporary inbound
and outbound block rules on every profile to the exact final Development and
Shipping launcher/runtime paths.

- Development completed S01 and save/load continuation at 1.140 ms p99 frame
  time with zero 8x backlog.
- Its 49 sampled TCP observations were only the expected local Development trace
  listener; no unexpected socket was observed.
- Shipping remained running for five seconds and produced zero TCP observations.
- The tested package hashes match the final package identity table above.
- The script exited 0, wrote `passed: true`, removed all eight temporary rules,
  and left no packaged process running.

## Acceptance-record enforcement

- `Phase1AcceptanceManifest.json` binds the closeout to package source commit
  `cdb1ad16e11a5b5a10fbb5a39072886242c9451b` and the four hashes above.
- The updated reference collector rehashed all four binaries before launch. Its
  five-second development-host harness passed package identity and runtime
  metrics at 1.816 ms p99, while correctly retaining `passed: false`.
- A minimal tester fixture and an all-positive rehearsal both retained
  `passed: false`; the latter failed only the deliberately absent formal-evidence
  attestation.
- The strict aggregate passed exact package identity, the full pipeline, and
  elevated network-denied S15. With no certifying external records, it reported
  exactly `referenceTierPassed` and `testerAcceptancePassed` as false.
- Calling the aggregate without `-AllowIncompleteEvidence` wrote the same false
  result and exited with failure. The switch affects exit handling only and
  cannot turn an incomplete result into acceptance.

## External phase-close evidence still required

1. Run `scripts/phase1/Test-Phase1ReferenceTier.ps1 -TierAttestation AtOrBelowApprovedTier`
   on a physical machine meeting or falling below the approved reference tier.
2. Complete the
   [unassisted new-tester protocol](../../docs/planning/50-production/phase-1-new-tester-protocol.md),
   including audible local speech and caption/local-cue fallback.
3. Run `scripts/phase1/Test-Phase1Acceptance.ps1` and require
   `AMSim/Saved/Phase1/acceptance-result.json` to report `passed: true`.

Until those two records pass, Phase 1 implementation and offline boundary are
verified but the formal phase gate remains open.
