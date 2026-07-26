# Phase 4 Content Provenance

**Status:** Approved
**Reviewed:** 2026-07-26
**Review ID:** `CT04.Phase4.RegionalWeek.2026-07-26`

Phase 4 contains 34 project-authored Primary Asset definitions under
`/Game/Phase4/Definitions`. Their stable IDs, display names, localization keys,
phase owner, definition kind, attributes, source, and provenance are authored
by `scripts/phase4/CreatePhase4Assets.py`.

## Source record

| Content family | Source | Runtime form | Decision |
| --- | --- | --- | --- |
| Operators, contracts, aircraft roles, gates, remote bus, transfer, border, transport, weather, incident, report, renewal, balance, scenario, and presentation definitions | Project-authored | `UAMSimPhase4Definition` Primary Assets | Approved |
| Timetable and incident presentation | Project-authored UMG plus deterministic Paper2D composition | Query-backed widgets and serialized sprite references | Approved |
| VA-04 and VA-06 concept art | Approved planning references | Not copied or cooked | Approved as non-runtime evidence |
| External aircraft files | None | None | Not imported |
| Generated Phase 4 texture or icon candidates | None retained | None | Not applicable |
| Required 3D gameplay assets | None | None | Prohibited |

The machine-readable companions are
`AMSim/Config/Phase4/Phase4ContentManifest.json` and
`AMSim/Config/Phase4/Phase4ContentReview.json`. The final Shipping IoStore
contains a name-for-name match for all 34 definitions. Runtime string asset
loading is prohibited, and any later external aircraft, generated icon,
surface, vehicle, or prop requires a new provenance review.
