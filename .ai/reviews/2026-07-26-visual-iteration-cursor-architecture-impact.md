# Visual Iteration and Cursor Architecture Impact

**Review type:** Architecture impact  
**Date:** 2026-07-26  
**Scope:** Cursor persistence, concept-art comparison workflow, and Phase 1 gate timing

## Findings

- Cursor policy belongs to the player-controller and CommonUI input-config boundary. A custom project controller establishes visible cursor, click, and hover defaults; the root activatable widget declares `All` input with capture that does not hide or lock the cursor.
- The change affects presentation input only. It does not alter simulation commands, fixed-step state, saves, checksums, or content identities.
- Visual comparison remains supporting evidence, subordinate to written behavior and accessibility requirements. A generated side-by-side board and structured mismatch worksheet improve review without turning pixel similarity into a gate.
- The four-hour soak infrastructure remains useful, but requiring it before subsequent development phases is disproportionate. Long-soak and formal external usability evidence can be deferred to release hardening while short automated and hands-on checks remain continuous.
- Image generation is a source-art workflow, not runtime architecture. Generated outputs still require provenance, review, 2D suitability, import validation, and explicit acceptance before becoming game content.

## Migration and rollback

- Existing saves and packages require no migration.
- Reverting the custom player controller restores the previous default input behavior.
- Comparison boards and worksheets are generated under ignored evidence directories and do not become runtime dependencies.
- Deferring external gates changes planning status only; existing collectors and acceptance scripts remain intact for later use.
