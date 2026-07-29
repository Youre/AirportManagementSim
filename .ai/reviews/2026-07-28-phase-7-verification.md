# Phase 7 Release-Hardening Verification

**Decision:** Release candidate ready for owner acceptance
**Candidate:** Riverbend RC1
**Source commit:** `24c17bc933c961d3e7119d70d4e4f32b010c50a6`
**Verified:** 2026-07-29 UTC

## Outcome

Phase 7 technical implementation passes. It adds schema-8 contextual-help
acknowledgement, first-use guidance, the five-tab Release Guide, local
accessibility settings, centralized captions-first radio, the reviewed release
catalog, actual-product concept validation, and reproducible release audits
without adding new airport simulation mechanics.

This decision does not close Phase 7. The owner deliberately deferred the
consolidated human test until the release candidate was available. The
prepared journeys, time-based tests, formal unassisted/audio/caption protocol,
and final visual approval remain pending.

## Automated verification

| Check | Result |
| --- | --- |
| Phase 7 focused automation | 5 succeeded; 0 warnings, failures, or not-run |
| Complete Phase 1-7 automation | 67 succeeded; 1 succeeded with warning; 0 failed/not-run/in-process |
| Retained warning | `RoundTripAndBackup` exercises the expected missing-primary recovery path |
| Save migration | Schemas 1-7 migrate to schema 8; schema-8 round-trip and invalid acknowledgement rejection pass |
| Project audit | Required definitions resolve; manifest dry run and prior catalogs pass |
| Rendered fixtures | Deterministic Phase 1 and Phase 4 journeys pass |
| Help scale matrix | 100%, 125%, 150%, 175%, and 200% pass; compact mode at 175-200% |
| Source organization | Every C++ source/header is at or below 2,000 lines |

## Package and runtime verification

Clean Unreal 5.8 BuildCookRun completed for Win64 Development and Shipping.
Each final IoStore contains 628 packages. The cook reference manifest contains
all 15 `/Game/Phase7/Definitions` assets with none missing.

The package audit reports:

- zero forbidden editor, MCP, toolset, test, Python, or RemoteControl runtime
  dependencies;
- zero direct runtime string asset loads;
- zero required 3D asset candidates;
- zero cooked unreviewed music files;
- zero secret, credential, or unexpected endpoint candidates;
- zero source line-limit violations.

The audio-enabled Development packaged S01 completed the journey and
save/load, produced five phrase intents, retained zero 8x backlog, averaged
1081.793 FPS, and recorded 1.255 ms p99 frame time. Development opened only
the expected local trace listener. Shipping stayed live for eight seconds and
opened zero TCP sockets.

The first packaged audio attempt exposed an initialization-order crash:
the game-instance radio service activated text-to-speech before Unreal's audio
device was fully ready. Provider initialization is now deferred until the first
real caption. The clean package was rebuilt and the audio-enabled journey and
shutdown then passed; the final result does not rely on `-NoSound`.

## Cook correction

The first package audit correctly rejected a cook that omitted Phase 7 assets.
`/Game/Phase7` was added to `DirectoriesToAlwaysCook`, the package audit was
changed to consume Unreal 5.8's authoritative cook reference set, and both
configurations were rebuilt from a clean cook. The final count is the Phase 6
613-package baseline plus exactly 15 Phase 7 definitions.

## Visual validation

Seven side-by-side boards use actual 1920 x 1080 Unreal captures, not surrogate
HTML or reconstructed UI. Each review records equivalent state, three
highest-impact differences, intentional specification overrides, and a bounded
correction.

The audit's largest correction made Terminal, Regional, Advanced, and Major
explicit destinations with Back navigation. Later capability eligibility no
longer replaces the concept-mapped screen under inspection. The implementation
now contains the required overview, construction, terminal-flow, timetable,
turnaround, incident, and six-path progression content. Remaining debt is
primarily illustration density, material detail, prop variety, and animation.

## Release identity

`AMSim/Config/Phase7/Phase7ReleaseManifest.json` records:

- source commit and release-content identity;
- exact Development and Shipping launcher/runtime hashes;
- all technical gates as passed;
- all owner gates as pending.

The machine-readable external records are:

- `AMSim/Saved/Phase7/pipeline-result.json`;
- `AMSim/Saved/Phase7/release-audit.json`;
- `AMSim/Saved/Phase7/packaged-runtime.json`;
- `AMSim/Saved/Phase7/release-attestation.json`;
- `AMSim/Saved/Phase7/AutomationFinal/index.json`;
- `AMSim/Saved/Phase7/Scale/scale-matrix.json`.

## Pending owner acceptance

The following remain required before Phase 7 can be marked complete:

1. six Phase 5 Advanced prepared journeys;
2. six Phase 6 Major prepared journeys;
3. the 15-minute maximum-load observation;
4. one genuine 10-15 hour pacing journey;
5. the four-hour physical reference-tier soak;
6. the formal unassisted tester, audio, caption, save/load, and comprehension
   protocol;
7. final concept-art visual approval.

Until those pass with no release blocker, the authoritative status is
`release candidate ready for owner acceptance`.
