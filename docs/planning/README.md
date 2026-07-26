# Planning Workspace

This directory is the durable source of truth for planning the 2D, top-down airport management game. It holds approved concepts, research, decisions, and later system specifications. It does not contain implementation code.

## Current planning stage

The project is in **phased implementation**. `ideas.txt` remains the living concept source; the approved specification set defines implementable feature behavior, technical boundaries, initial tuning, verification, and phased integration. Phase 0 is complete, and the approved Phase 1 grass-airfield plan is ready for work packages P1-00 and P1-01. Work still enters only through its owning approved specifications and phase gate.

## Directory map

| Path | Purpose |
| --- | --- |
| `00-foundation/` | Vision, pillars, scope, audience, and product decisions |
| `10-player-experience/` | Player journeys, onboarding, controls, UX, accessibility, and feedback |
| `20-game-systems/` | Economy, construction, aviation, passengers, weather, progression, and simulation rules |
| `30-content-and-assets/` | Aircraft, buildings, vehicles, audio, visual language, and content pipelines |
| `40-technical-design/` | Unreal architecture, data models, save format, performance, and MCP-assisted workflows |
| `50-production/` | Milestones, vertical slices, testing strategy, risks, and release planning |
| `90-research/` | Competitor research, source material, screenshots, and inventories |

## Document lifecycle

Every substantive planning document should declare one of these states near its title:

- `Draft`: actively being explored; contradictions are allowed.
- `Review`: coherent enough for focused critique.
- `Approved`: current planning baseline.
- `Superseded`: retained for history and linked to its replacement.

Use dates in ISO format (`YYYY-MM-DD`). Record unresolved choices explicitly rather than silently inventing requirements. When a decision changes the game's identity, progression, scope, data contracts, or asset direction, also record it in `.ai/context/decision-log.md`.

## Standard document shape

New planning documents should normally include:

1. status, owner, and last-updated date;
2. purpose and player-facing outcome;
3. in-scope and out-of-scope boundaries;
4. proposed behavior or rules;
5. dependencies and cross-system effects;
6. risks, edge cases, and accessibility considerations;
7. open questions;
8. acceptance criteria for moving the document to `Approved`;
9. sources and links.

The level of detail should match the stage. Concept documents explain the experience and boundaries. System documents later define testable rules. Technical documents should not make game-design decisions implicitly.

## Project constraints

- Engine: Unreal Engine, with Unreal's MCP server used for inspection and editor-assisted workflows once the installation is available.
- Presentation: 2D cartoon art viewed primarily from above.
- 3D assets and a 3D presentation are out of scope.
- Reuse eligible 2D aircraft art from `C:\Users\dave\Documents\Joes_Game\dist\assets`; treat that deployment directory as read-only and do not copy from it until the asset-import plan is approved.
- Real aircraft names, dimensions, performance, liveries, and branding require a later accuracy and rights review.
- Numerical values in `ideas.txt` are concept placeholders until the economy is planned and balanced.

## Aircraft inventory interpretation

The research inventory retains 46 canonical source records. On the replacement workstation, `C:\Users\dave\Documents\Joes_Game\dist\assets` currently exposes 45 matching transparent PNGs with build-hash suffixes. Their dimensions match the corresponding inventory records.

- 23 catalogued civilian/commercial sprites are core reuse candidates.
- 14 catalogued military sprites are excluded from core progression.
- 9 additional aircraft sprites exist without entries in the inspected commercial or military catalogs.
- 2 military catalog entries (P-40E and C-141B) have no matching file in the sprite directory.
- The inventoried Airbus A330-300 SVG is not present in the replacement deployment and is unavailable for import until separately restored and reviewed.
- Build-hash suffixes are deployment details. The import manifest must capture the exact deployed filename and checksum used rather than assuming a stable physical filename.

The inventory confirms presence, format, dimensions, and source classification. It does not approve rights, accuracy, import settings, in-game scale, or visual consistency.

## Active documents

- [Specification index](00-foundation/specification-index.md) — normative entry point and ownership map
- [Implementation planning review record](00-foundation/specification-review-record.md) — architecture and adversarial review
- [Phased implementation roadmap](50-production/phased-implementation-roadmap.md)
- [Phase 1 grass-airfield implementation plan](50-production/phase-1-grass-airfield-implementation-plan.md)
- [Phase 1 reference-hardware gate](50-production/phase-1-reference-hardware.md)
- [Phase 1 unassisted new-tester protocol](50-production/phase-1-new-tester-protocol.md)
- [Verification and requirements traceability](50-production/verification-traceability.md)
- [Initial Game Concept](00-foundation/game-concept.md) — earlier concept summary
- [`ideas.txt`](../../ideas.txt) — living concept source
- [Concept art reference set](30-content-and-assets/concept-art/README.md) — controlled visual validation references
- [Aircraft asset inventory](90-research/aircraft-asset-inventory.csv) — research data, not reuse approval

All feature and technical documents linked from the specification index are active approved baselines. Begin implementation work by reading the index, the roadmap phase being opened, and every owning specification named for that phase.
