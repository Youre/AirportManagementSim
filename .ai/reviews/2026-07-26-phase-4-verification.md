# Phase 4 Verification Record

**Date:** 2026-07-26
**Engine:** Unreal Engine 5.8.0
**Platform:** Win64
**Branch:** `codex/planning-docs-concept-art`
**Result:** Passed

## Verified outcome

Phase 4 adds a deterministic regional scheduled-airport week with three
fictional recurring operators, 21 exact five-minute flights, contact Gates A1
and A2, remote Stand R1 with bus boarding, early/late/weather operation, a
locked-horizon gate change, passenger and bag transfers, international border
processing, rental/rail access, a warned serious incident, recovery, and tenant
renewal.

The complete fixture records 21 completed flights, 18 connected passengers,
6 missed and rebooked passengers, 32 completed transfer bags, 96 border-processed
international passengers, 42 rental-car passengers, 58 rail passengers, one
accepted renewal, and a recoverable `No injuries` incident outcome.

## Build, automation, and persistence

| Gate | Result |
| --- | --- |
| `AMSimEditor Win64 Development` | Passed |
| Focused `AMSim.Phase4` automation | 5 passed, 0 failed |
| Full `AMSim` automation | 45 passed plus 1 retained warning, 0 failed/not-run/in-process |
| Deterministic regional fixture | Identical command streams produce identical checksums |
| Persistence | Schema-5 round-trip, schema-4 migration, future rejection, broken-reference rejection, and incident-provenance rejection passed |
| Safety invariant | No serious incident materializes without a stored and acknowledged warning |
| Presentation | Revision-gated view mapping, cooker-visible Paper2D assets, pooled response proxies, and typed UI commands passed |

## Rendered and packaged evidence

The machine-readable local records are:

- `AMSim/Saved/Phase4/smoke-result.json`;
- `AMSim/Saved/Phase4/Scale/scale-matrix.json`;
- `AMSim/Saved/Automation/Phase4Complete/index.json`;
- `AMSim/Saved/Automation/Phase4Full/index.json`;
- `AMSim/Saved/Phase1/pipeline-result.json`;
- packaged Phase 4 smoke under
  `AMSim/Saved/Phase1Packages/Development/Windows/AMSim/Saved/Phase4/`.

The final gate verifies:

- 1920 x 1080 VA-04, VA-06, and completed-week captures at 100%, 125%, 150%,
  175%, and 200% UI scale;
- full rail layouts at 100-150% and compact, data-derived labels/drawers at
  175-200%;
- zero 8x backlog, exact save/load continuation, and recoverable completion;
- clean Development and Shipping builds;
- packaged Development S06/S10 journey and save/load;
- five-second Shipping launch with zero TCP sockets;
- zero forbidden editor, test, MCP, Python, RemoteControl, or Toolset files;
- zero Phase 4 runtime string asset loads and zero required 3D candidates;
- direct Shipping IoStore inclusion of all 34 Phase 4 assets.

The packaged Phase 4 smoke passed at 1.722 ms p99, 1,086.460 average FPS,
535 MiB maximum resident memory, zero backlog, and checksum
`4002092208679843671`.

The first package attempt was rejected after cook because staging lost its
local Zen oplog connection. Restarting the persistent UE 5.8 Zen service and
rerunning from a clean package root produced successful Development and
Shipping archives. No implementation, test, cook-content, or package-boundary
assertion was waived.

## Visual review

VA-04 and VA-06 combined boards drove aircraft identity on each flight card,
shorter incident copy, an expanded compact footer, and 175-200% data-derived
labels. The remaining difference is deliberate project-owned schematic art
density, not a hidden simulation or interaction gap. See
[the Phase 4 visual review](../../docs/planning/50-production/phase-4-visual-review.md)
and the project-root `design-qa.md`.

## Deferred release evidence

The four-hour physical reference-tier soak and formal unassisted new-tester
protocol remain Phase 7 gates. They are not Phase 4 development blockers.
