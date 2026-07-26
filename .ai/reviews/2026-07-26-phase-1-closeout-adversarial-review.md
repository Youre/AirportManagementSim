# Phase 1 Closeout Hardening Adversarial Review

**Status:** Passed with external gates retained
**Date:** 2026-07-26

## Findings

### Automation process exit was insufficient evidence

Unreal returned exit code 0 for a focused failing automation assertion. The Phase 1
pipeline now requires `Automation/index.json` and rejects failed, not-run, or
in-process tests. The final report contains 25 tests, zero in those categories.

### Flight catch-up could omit effects

A rendered retry completed with four rather than five phrase intents. Direct
fixed-step tests did not cover a catch-up leap. Progression now walks every
intermediate state, and a forced leap verifies all five phrases, inspection,
fueling, reward recognition, and completion.

### Performance proof could be broad but indirect

The previous packaged result measured rendered frames while paused and did not
name 1x simulation-work percentiles or exact scalability. The final measurement
runs at 1x, records median/p99 simulation work, and verifies the named reference
profile.

### A short run could be mislabeled as the formal soak

The portable collector reports `passed: true` only with an
`AtOrBelowApprovedTier` attestation, a 14,400-second result, at least 200 memory
samples, passing budgets, and bounded memory trend. Its five-second host check
correctly reports false.

### Human fallback could become a developer assertion

The forced-speech-failure switch proves an executable fallback path but does not
prove audibility or comprehension. The acceptance protocol requires a genuinely
new tester, zero hints, four correct explanations, normal local speech, equivalent
captions, and forced fallback comprehension.

### External evidence remains external

The first UAC request for S15 was canceled and was correctly retained as skipped.
A user-requested retry subsequently passed against the final package hashes with
exact-executable inbound/outbound denial and complete rule cleanup. The current
RTX 5090 host is still not the approved physical tier, and no unassisted tester
record exists. Those two gates remain open and are not inferred from automated or
developer-host evidence.

## Accepted warning

The automation report has one success-with-warning: the Phase 0 backup-recovery
fixture intentionally removes the current snapshot, so the failed first read is
expected before backup load succeeds.

## Resumed-goal evidence review

### A text template could be completed ambiguously

The tester protocol now uses a bounded JSON recorder. It accepts only an
anonymous identifier, tester band, timestamps, booleans, and non-personal
observation/follow-up codes. A child-band pass requires consent attestation. It
does not accept names, prose notes, audio, images, contact details, or save data.

### A rehearsal could be mistaken for external evidence

The recorder requires an explicit `RecordFormalEvidence` attestation. Without
it, every other positive criterion still produces `passed: false` and
`evidenceKind: non-certifying-record`. Failed real sessions can be preserved but
cannot pass.

### Evidence could refer to a different package

The reference collector and tester recorder hash the Development launcher,
Development runtime, Shipping launcher, and Shipping runtime against the
tracked acceptance manifest. The aggregate independently rehashes the local
packages and rechecks the hashes embedded in both external records.

### An incomplete aggregate could be reported as closure

The strict aggregate exits with failure unless package identity, pipeline,
network-denied, physical reference-tier, and formal tester checks all pass.
`-AllowIncompleteEvidence` changes only the command exit behavior; the JSON
continues to report `passed: false` and names each missing or failed gate.
