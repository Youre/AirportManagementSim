# Phase 7 Architecture-Impact Review

**Status:** Controls approved for implementation  
**Date:** 2026-07-28  
**Scope:** PI-10 release hardening, guidance, accessibility, audio, content,
migration, and release evidence

## Decision

Phase 7 is a release layer, not a seventh gameplay domain. Phase 1-6 continue
to own airport simulation. The only authoritative save change is a bounded
set of acknowledged contextual-help IDs in Phase 1 state, requiring schema 8.

## Ownership

- `AMSimSimulation` owns typed help acknowledgement, deterministic state,
  checksum participation, restore validation, and schema-8 migration.
- `AMSimGameplay` owns Phase 7 Primary Assets, release catalog validation,
  save/package orchestration, and release identity.
- `AMSimUI` owns the query-derived contextual card, Help/accessibility
  composition, local accessibility profile, and one game-instance offline
  radio service.
- `AMSimEditor` and scripts own asset creation, audit, cook inventory,
  screenshot capture, package evidence, and release-manifest assembly.
- Documentation owns the human protocols and records their open/pass status
  without converting automation into human evidence.

## Invariants

- Help acknowledgement cannot change gameplay state, progression, balance, or
  command ordering outside its own event.
- Accessibility settings are local and never leak progression between saves.
- Captions are visible before speech and remain complete during speech failure.
- No network, backend, account, telemetry, unapproved content, 3D content, or
  runtime string asset load is introduced.
- The product remains strict top-down 2D and the existing Riverbend component
  language remains the presentation source of truth.
- The Phase 1-6 serializers and near-limit UI/presenter files remain below the
  2,000-line standard through focused new files.

## Persistence and rollback

Schema 8 appends help acknowledgement after the Phase 1 body. Schemas 1-7
migrate with an empty set. Restore rejects empty, duplicate, unknown, or
excessive help IDs. Rollback requires restoring a schema-7 backup; no
down-migration is attempted.

The config-backed accessibility profile is independently removable and
contains only scale/reduced-motion preferences.

## Required verification

- focused command, checksum, restore, schema-8, and schemas 1-7 migration;
- local-profile defaults, bounds, persistence-safe fields, and immediate
  production-shell reapplication;
- phrase-family completeness, deduplication, speech failure, and captions-first
  behavior;
- Phase 7 Primary Asset, cook, provenance, and release-manifest validation;
- full automation, deterministic rendered smokes, actual screenshot audit,
  clean Development/Shipping packages, offline socket observation, security
  scans, and source line limits.

With these controls, PI-10 fits the approved deterministic, persistence,
offline, child-safety, accessibility, content, and presentation architecture.
