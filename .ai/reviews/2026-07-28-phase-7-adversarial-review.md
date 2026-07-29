# Phase 7 Adversarial Review

**Status:** Preliminary risks locked; final review pending verification  
**Date:** 2026-07-28  
**Scope:** PI-10 release candidate

## Review question

Could the project look release-ready while guidance, captions, migration,
rights, offline behavior, concept-art alignment, or human acceptance remains
decorative or unproven?

## Risks and countermeasures

### A polished Help screen could still be disconnected from play

Contextual help is selected from the current authoritative Phase 1 snapshot.
Dismissal submits a typed command and persists per save. Every dismissed item
also appears in the full Help surface.

### Accessibility controls could be labels with no effect

Scale choices update the engine application scale, persist in a local profile,
and reconstruct the production shell. Compact behavior is recaptured at
175-200%. Reduced motion is meaningful because release UI must not introduce
essential flashing or animated-only state.

### Speech could remain Phase-1-only or fail silently

One game-instance service owns provider lifecycle and deduplication. Phase 1
phrase intents and later operational captions use it. Captions render
independently and the forced-failure path uses the local cue.

### Schema 8 could corrupt accepted saves

The new payload is append-only and schema-gated. Retained schemas 1-7 migrate
with empty help history. Corrupt, duplicate, unknown, excessive, and future
state are rejected before world activation.

### A release manifest could bless missing or unreviewed content

Manifest entries resolve to cooked Primary Assets and carry stable IDs,
localization keys, phase, provenance, 2D, runtime-load, and rights status.
Unreviewed music and unapproved aircraft are explicitly excluded.

### Static visual boards could be mistaken for implementation

Phase 7 visual evidence uses screenshots produced by the running Unreal
product in the current audit. Illustrative boards cannot satisfy the gate.
Each image is inspected against the corresponding concept reference and
written CT-05 requirements.

### Automated evidence could be mislabeled as human acceptance

The six Advanced journeys, six Major journeys, 15-minute observation, pacing
journey, four-hour soak, unassisted protocol, and final visual checkpoint
remain open until the owner performs them against recorded package hashes.

### Shipping could contain tools, endpoints, secrets, or unapproved assets

Source and package scans cover filenames, receipts, configs, readable
manifests, runtime load calls, network endpoints, credential patterns, 3D
types, operator branding, aircraft manifests, and IoStore inventory. A
network-observed offline smoke is retained.

## Final exit condition

Close this review only after focused/full automation, actual visual capture
inspection, clean packages, cook/security/offline audits, migration matrix,
rights inventory, and source-organization checks pass. Phase 7 itself remains
open until the owner acceptance records also pass.
