# Phase 5 Content Provenance

**Status:** Approved  
**Reviewed:** 2026-07-27  
**Review ID:** `CT01.Phase5.CargoBreadth.2026-07-27`

Phase 5 contains 44 project-authored Primary Asset definitions under
`/Game/Phase5/Definitions` plus two textures and two PaperSprite assets under
`/Game/Phase5/Presentation/Aircraft`.

| Content family | Source | Runtime form | Decision |
| --- | --- | --- | --- |
| Cargo, warehouse, vehicle, tenant, event, balance, scenario, aircraft, and presentation definitions | Project-authored by `scripts/phase5/CreatePhase5Assets.py` | `UAMSimPhase5Definition` Primary Assets | Approved |
| Riverlark F28 feeder freighter | Project-authored editable SVG and deterministic raster export | `T_Riverlark_F28`, `S_Riverlark_F28` | Approved |
| Hearthwing F62 regional freighter | Project-authored editable SVG and deterministic raster export | `T_Hearthwing_F62`, `S_Hearthwing_F62` | Approved |
| Phase 5 Figma validation frames | Project-authored validation source | Repository PNG exports; not cooked | Approved as evidence |
| VA-01, VA-03, VA-04, VA-05, VA-06, VA-07 | Approved planning references | Not copied or cooked | Approved as non-runtime evidence |
| `Joes_Game` aircraft files | None used | None | Prohibited for Phase 5 |
| External or military aircraft | None | None | Not imported |
| Required 3D gameplay assets | None | None | Prohibited |

The editable aircraft masters and raster exports are retained under
`SourceAssets/Phase5/Aircraft`. They use fictional names and liveries,
transparent backgrounds, top-down silhouettes, reviewed pivots, and
presentation-time heading rotation. The original SVG masters remain outside
cooked content.

| Source file | SHA-256 |
| --- | --- |
| `Riverlark_F28.svg` | `aec23de8e091f12feddc511df09d29a22c9cf271d740bfae88938fae2dfc3d51` |
| `Riverlark_F28.png` | `a73f15ba108ad9581d801eb42c260f28a6c8dede2658b06842126abe14d5fba2` |
| `Hearthwing_F62.svg` | `867dd192c92aa6b4765e0bd32b8c4d1e2fdeb3a1ba11a9995b836d1ea997bb89` |
| `Hearthwing_F62.png` | `bfa35ecedbf0103bb768d587a256abba1dcb088659946a1f673bfb3cf4409bf1` |

The final Shipping IoStore contains all 48 Phase 5 assets. Its directory index
reports 50 Phase 5 entries because each of the two texture assets also has one
bulk-data entry. The production root Widget Blueprint and both freighter
texture/sprite pairs are present.

Runtime string loading is absent from Simulation, Gameplay, and UI modules.
The only `LoadObject` calls are confined to the editor-only presentation asset
commandlet used during authoring.

