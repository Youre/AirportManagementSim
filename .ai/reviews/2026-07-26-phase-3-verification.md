# Phase 3 Verification Record

**Date:** 2026-07-26
**Engine:** Unreal Engine 5.8.0
**Platform:** Win64
**Branch:** `codex/planning-docs-concept-art`
**Result:** Passed

## Verified outcome

Phase 3 adds the first complete domestic passenger-airport vertical slice.
One player-built terminal connects landside, public pedestrian, controlled,
gate, arrivals, and baggage networks. RB 304 processes 28 departing and
24 arriving passengers plus 34 checked bags through simultaneous flow with
exact reconciliation, no sterile bypass, and a completed accessible journey.

The slice adds persistent terminal construction, named parties and compact
needs, domestic security, passenger and bag queues, staff teams, a fictional
tenant/operator, curb/parking/taxi/bus capacity, schema-4 persistence, a
query-backed terminal presenter, and pooled Paper2D world presentation. Phase 1
remains the economy owner and Phase 2 remains the living-airport owner.

## Build, audit, and automation

| Gate | Result |
| --- | --- |
| `AMSimEditor Win64 Development` | Passed |
| Project audit | Passed; 20 Phase 1, 33 Phase 2, and 34 Phase 3 Primary Assets resolved |
| Focused `AMSim.Phase3` automation | 7 groups passed, 0 failed |
| Full `AMSim` automation | 40 passed, 1 passed with retained warning, 0 failed/not-run/in-process |
| Complete S05 journey | 52/52 passengers and 34/34 bags completed; one exact reconciliation pass |
| Security and accessibility | Opening validation, controlled boundary, no bypass, and accessible route passed |
| Persistence | Schema-4 round-trip plus schema 1-3 migration and six boundary continuations passed |
| Scale fixture | 10,000 logical and 2,000 visible proxy/pooling contract passed |
| Presentation isolation | Revision gating, proxy reuse/sorting, state mapping, and no simulation mutation passed |

## Rendered and packaged evidence

The final machine-readable records are:

- `AMSim/Saved/Phase1/pipeline-result.json`;
- `AMSim/Saved/Phase1/smoke-result.json`;
- `AMSim/Saved/Phase3/pipeline-result.json`.

The complete pipeline verifies:

- 1920 x 1080 operation at 100%, 125%, 150%, 175%, and 200% UI scale;
- VA-03 complete-terminal and Phase 3 completion captures at every scale;
- bounded compact drawers and concise labels at 175-200%;
- zero 8x simulation backlog;
- clean Development and Shipping builds;
- packaged Development S05 smoke/save/load and five-second Shipping launch;
- zero unexpected Development sockets and zero Shipping TCP sockets;
- zero forbidden editor, test, MCP, Python, RemoteControl, or Toolset matches;
- zero Phase 3 runtime string asset loads;
- zero required 3D gameplay candidates;
- a direct Shipping IoStore name match for all 34 Phase 3 assets.

The final packaged smoke ran at 1,108.093 average FPS with 1.723 ms p99 frame
time, 534 MiB maximum resident memory, zero 8x backlog, and checksum
`10622603748974364525`.

The first package attempt exposed an Unreal 5.8 build-tool sequencing issue:
the owner-bound Zen cook process exited before staging read its oplog. The
pipeline now starts Zen independently before `BuildCookRun`; Development and
Shipping then cook, stage, package, archive, and exercise successfully.

## Visual review

The VA-03 equivalent-state board and design QA drove two bounded corrections:
semantic warm/cool/purple/amber/green room roles with visible concurrent
proxies, and non-overlapping 175-200% drawers. Details and intentional
project-owned proxy-art differences are recorded in
[the Phase 3 visual review](../../docs/planning/50-production/phase-3-visual-review.md)
and the project-root `design-qa.md`.

## Deferred release evidence

The four-hour physical reference-tier soak and formal unassisted new-tester
protocol remain Phase 7 gates. They are not Phase 3 development blockers.
