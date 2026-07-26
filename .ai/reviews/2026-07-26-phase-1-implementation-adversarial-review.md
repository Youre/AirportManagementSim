# Phase 1 Implementation Adversarial Review

**Status:** Passed with external acceptance locks
**Date:** 2026-07-26
**Scope:** PI-03 implementation, regression, save safety, UI truthfulness, package boundary, and evidence integrity

## Review question

Could the implemented slice appear complete while omitting secondary contracts, skipping transient states on a fast host, mis-typing content assets, allowing presentation-owned outcomes, leaking editor/network dependencies, or presenting unverified workstation results as phase-close evidence?

## Findings resolved

### Secondary contracts were initially narrower than PI-03

The first integrated journey implemented the S01 accept path but not cancellation/refund, closure/reopen, offer pin/decline, or explicit stand-buffer ownership. The final implementation adds typed commands/events/state, schema persistence, UI controls, and `AMSim.Phase1.Contracts.SecondaryCommandLifecycle`.

### Catalog type shadowing could hide missing content

An anonymous facility Primary Asset type identifier shadowed the facility definition member. It was renamed, all assets were re-authored/resaved, and the commandlet now resolves exactly 20 required Primary Assets with their expected stable types.

### Fast rendered smoke could skip evidence states

Running at 8x too early could pass final assertions without capturing Building or Turnaround. The smoke now uses 1x through those transient screenshots, switches to 8x only afterward, and requires all VA-01/02/04/05 plus completion captures.

### UI-scale pass flags did not prove readable composition

Screenshot review found a wrapped objective overlapping the next section at 100% and a long history block colliding with rating text. A stable objective layout region and concise query-backed history summary resolved both. Local rail scrolling covers 100–150%; stacked page scrolling covers 175–200%.

### The first aggregate runner raced the rendered editor

PowerShell returned before the GUI process completed, so an early runner could look for evidence too soon. The runner now explicitly waits for the rendered editor process and fails on its exit code or absent result.

### Timetable buffers were initially explanatory text only

The final flight record stores stand occupancy start/end around the scheduled arrival/departure operation. Scheduling requires the stable Stand A1 definition, rejects an incompatible stand, persists the buffer window, and renders those exact values.

## Negative-path evidence

Automated tests cover blank name, wrong map, early construction, invalid/disconnected geometry, insufficient/invalid commands, unsupported speed, incompatible stand, late cancellation, active-flight closure, schema migration, boundary continuation, corruption/backup behavior, recovery, deterministic replay, reservation ownership, service readiness, and phrase-caption completeness.

The final full suite passed 24 of 24 tests: 14 preserved Phase 0 contracts and 10 Phase 1 contracts.

## Package and offline review

Development and Shipping packages build and launch from clean archives. The packaged S01 completes with save/load continuity. Shipping has zero forbidden file/text/receipt matches and zero observed TCP sockets. Development observations were only the expected local trace listener; unexpected observations were zero.

The current process was not elevated, so firewall rules were not applied. `scripts/phase1/Test-Phase1NetworkDenied.ps1` is the exact elevated follow-up and this review does not relabel socket observation as network-denied proof.

## External acceptance locks

- Run the final packages through elevated inbound/outbound firewall denial.
- Measure on a physical machine meeting or falling below the approved reference tier.
- Have a new tester complete and explain the journey without developer intervention, including audible local radio and caption/fallback comprehension.

No code or package-boundary defect remains open from this review. The phase remains open only for those externally sourced acceptance records.
