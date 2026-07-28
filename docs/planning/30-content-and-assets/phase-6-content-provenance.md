# Phase 6 Content Provenance

**Status:** Approved for technical baseline
**Reviewed:** 2026-07-28
**Review ID:** `CT01.Phase6.MajorScale.2026-07-28`

Phase 6 contains 28 project-authored Primary Asset definitions under
`/Game/Phase6/Definitions`, 16 Riverbend Longreach 787-9 heading textures,
and 16 matching PaperSprite assets under `/Game/Phase6/Presentation`.

| Content family | Source | Runtime form | Decision |
| --- | --- | --- | --- |
| Major facilities, runway, operation, incident, repair, balance, scenario, and presentation definitions | Project-authored by `scripts/phase6/CreatePhase6Assets.py` | `UAMSimPhase6Definition` Primary Assets | Approved |
| Riverbend Longreach 787-9 | One owner-approved external silhouette reference followed by a project-authored editable SVG and deterministic raster exports | 16 texture/PaperSprite heading pairs | Approved as River & Sun |
| Phase 6 equivalent-state frames | Project-authored HTML source and deterministic Chrome exports | Repository PNG evidence; not cooked | Approved as technical comparison evidence; final owner visual checkpoint open |
| VA-01 through VA-07 | Approved planning references | Not copied or cooked | Approved as non-runtime evidence |
| Other `Joes_Game` aircraft | None used | None | Prohibited for Phase 6 |
| Real airline branding, military aircraft, or required 3D gameplay assets | None | None | Prohibited |

## Aircraft intake

Only
`C:\Users\dave\Documents\Joes_Game\dist\assets\Boeing_787-9-B24vQXZX.png`
was approved. A byte-identical non-cooked reference copy is retained as
`SourceAssets/Phase6/Aircraft/Boeing_787-9_source-approved.png`. Approval does
not extend to any other file in the source directory.

The external pixels and real-operator treatment are not runtime content.
`Riverbend_Longreach_787-9.svg` is the production source of truth: a strict
top-down, fictional River & Sun livery normalized to the documented Boeing
787-9 dimensions at 32 pixels per meter. The deterministic export generates
16 clockwise headings at 22.5-degree intervals with centered pivots.
The 62.8 m length and 60.1 m wingspan values were checked against
[Boeing's 787 technical characteristics](https://www.boeing.com/commercial/787).

| Source file | SHA-256 |
| --- | --- |
| `Boeing_787-9_source-approved.png` | `6aa56cd2cdd7035e3ee3526cff5a603751e03441da11ece6c7e6e550b9b04449` |
| `Riverbend_Longreach_787-9.svg` | `fe42cafbf5315b6922ff660570382a676c61d11d5bdbd7fb48ebaa1bf28f15b1` |
| `Riverbend_Longreach_787-9_master.png` | `988c11f97569d9e170d8774e992b3aef6b36cd2610b8b44598c71f701454ec40` |
| `Riverbend_Longreach_787-9_heading-review.png` | `c52807cb00b3c2d533e562af5c33b943de7431469bd016474c281dc055239c1b` |

The complete per-heading dimensions, rotations, pivots, and checksums live in
`SourceAssets/Phase6/Aircraft/aircraft-manifest.json`.

## Vector workflow decision

The automatic vector-composer trace under
`SourceAssets/Phase6/VGC/787-source-trace` is retained as provenance. Its
orchestration and artifact-health checks passed, but strict source-coverage
validation rejected its visual fidelity. It is not a production master or
runtime source. The clean semantic SVG was authored from the approved
silhouette and visual requirements instead.

## Cook and dependency evidence

Direct Shipping IoStore inspection reports all 60 Phase 6 assets, represented
by 64 entries because four texture bulk-data entries are listed separately.
The production `WBP_AMSimRootScreen` is also present. Runtime modules contain
zero direct string-path asset loads; Phase 6 uses serialized/construction-time
cooker-visible references and the reviewed Asset Manager catalog. Shipping
contains zero forbidden editor/MCP/test dependencies, zero required 3D
assets, and zero observed TCP connections.
