# Phase 3 Content Provenance

**Status:** Approved
**Reviewed:** 2026-07-26
**Review ID:** `CT04.Phase3.TerminalFlow.2026-07-26`

Phase 3 contains 34 project-authored Primary Asset definitions under
`/Game/Phase3/Definitions`. Their stable IDs, display names, localization keys,
phase owner, definition kind, attributes, source, and provenance are authored
by `scripts/phase3/CreatePhase3Assets.py`.

## Source record

| Content family | Source | Runtime form | Decision |
| --- | --- | --- | --- |
| Terminal, network, security, landside, staff, tenant, operator, contract, balance, scenario, and presentation definitions | Project-authored | `UAMSimPhase3Definition` Primary Assets | Approved |
| Terminal room/route/status/proxy presentation | Project-authored deterministic Paper2D composition | Serialized sprite references and pooled sprite components | Approved |
| Regional passenger aircraft presentation | Reviewed fictional Riverbend silhouette policy | Paper2D sprite reference | Approved for Phase 3 |
| VA-03 concept art | Planning reference only | Not copied or cooked | Approved as non-runtime evidence |
| External aircraft files | None | None | Not imported |
| Generated Phase 3 texture candidates | None retained | None | Not applicable |
| Required 3D gameplay assets | None | None | Prohibited |

The machine-readable companion is
`AMSim/Config/Phase3/Phase3ContentManifest.json`. The Shipping inventory must
contain a name-for-name match for all 34 definitions. Runtime string asset
loading is prohibited, and all future external or generated additions require
a new provenance review.
