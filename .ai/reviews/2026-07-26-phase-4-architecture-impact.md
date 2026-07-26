# Phase 4 Architecture-Impact Review

**Status:** Passed with mandatory implementation controls
**Date:** 2026-07-26
**Scope:** PI-06 regional scheduled-airport vertical slice

## Decision

Phase 4 adds one focused deterministic domain to the existing simulation
composition. It does not add a runtime module, replace the 250 ms fixed step,
duplicate Phase 1 economy authority, duplicate Phase 2 persistent-airframe
authority, or move Phase 3 passenger truth into presentation.

## Ownership

- `AMSimSimulation` owns Phase 4 contracts, timetable instances, connections,
  transfer bags, border/access summaries, forecast/approach state, renewals,
  incident audit, commands, events, queries, checksum, and schema-5 state.
- `AMSimGameplay` owns Phase 4 content resolution, lifecycle adapters, fixtures,
  packaged smoke, and save orchestration.
- `AMSimUI` owns a query-only regional-operations presenter and 2D operational
  overlays.
- `AMSimEditor` owns Phase 4 content, cooking, provenance, and dependency audits.
- `AMSimTests` owns development-only domain, persistence, presentation, content,
  and package assertions.

No Actor, Widget, PaperSprite component, Blueprint, screenshot, or editor/MCP
action authorizes a flight, connection, bag, border, weather, gate, tenant, or
incident outcome.

## Persistence impact

Schema 5 adds the complete Phase 4 domain. Schemas 1-4 migrate to an empty
Phase 4 state. Restore validation covers stable IDs, ownership, exact slots,
gate exclusivity, connection/bag references, remote boarding, border summaries,
approach restrictions, incident provenance, completion reconciliation, and
future schemas.

Selected flight, active tab, open drawer, widget animation, proxy assignment,
weather-particle placement, and overlay animation remain presentation state.

## Cross-domain impact

Phase 4 reads Phase 3 readiness and references its flight/passenger/bag boundary
without rewriting Phase 3 records. Bounded Phase 4 setup, operating revenue,
incident expense, and recovery aid post through Phase 1's existing external
economy ledger. Phase 2 airframes remain the persistent aircraft source of
truth; Phase 4 stores schedule references and operational summaries only.

## Scale impact

The initial regional fixture contains a bounded seven-day timetable and small
inspectable connection cohorts. Update work is bucketed by fixture day and
stable flight ID. Presentation virtualizes timetable cards and uses bounded
pooled world cues rather than authoritative per-flight, per-passenger, or
per-bag Actor Tick.

## UI impact

The root-screen source is already near the repository file-length limit.
Phase 4 therefore uses a separate regional presenter hosted by the existing
terminal surface. It reuses the accepted theme and input policy, preserving the
visible cursor, query-revision refresh, captions, and compact-scale rules.

VA-04 and VA-06 define state-specific compositions, not a second theme. Their
deep navy, rounded grouping, world/UI hierarchy, localized risk, adjacent
evidence, and cause/consequence/recovery grammar remain shared.

## Content and package impact

Phase 4 content is fictional and project-authored. No external aircraft file is
copied. `/Game/Phase4` is an explicit cooker root and Primary Asset scan path.
The Shipping IoStore inventory must match the source catalog. Runtime string
asset loading and required 3D gameplay assets are prohibited.

## Security and safety impact

Passenger data remains fictional and local. Customs and immigration are
abstract and cannot use protected traits. The serious incident records warned
risk and mitigations, reports only abstract harm, depicts no suffering, and
retains a recovery path. No backend, telemetry, microphone, account, or network
service is added.

## Mandatory controls

- Keep new source files focused and below 2,000 lines.
- Preserve Phase 1 economy, Phase 2 airframe, and Phase 3 passenger/bag
  authority through typed integration boundaries.
- Validate gate, slot, connection, transfer-bag, border, and incident invariants
  on command and restore.
- Require warning provenance before a serious incident can materialize.
- Reconcile all regional populations and bags at every completed-fixture state.
- Fail content audit/cooking on missing Phase 4 assets.
- Run equivalent-state VA-04 and VA-06 comparisons before visual acceptance.
- Prove every exit-critical command through normal player-facing UI.

With these controls, Phase 4 fits the approved module, save, UI, 2D, offline,
child-safety, and performance architecture.

## Exit evidence

All mandatory controls passed. New focused source files remain below 2,000
lines; schema 5, stable references, deterministic replay, warning provenance,
population/bag reconciliation, player-facing commands, content audit, and
VA-04/VA-06 comparisons are automated or recorded. Clean Shipping directly
contains all 34 Phase 4 assets and contains no forbidden editor/MCP/test,
runtime string-load, required 3D, or external Phase 4 aircraft dependency.
