# Visual Iteration and Cursor Adversarial Review

**Review type:** Adversarial  
**Date:** 2026-07-26  
**Scope:** Cursor persistence, visual comparison, generation policy, and deferred long testing

## Risks and controls

### CommonUI can overwrite controller input mode

Setting `bShowMouseCursor` only on the controller is insufficient because an activated CommonUI screen can apply its own capture policy. The root screen therefore also declares a cursor-preserving `FUIInputConfig`.

### A visible cursor can break camera input

The selected mode is `GameAndUI`, not UI-only. Mouse capture occurs only during mouse-down, the cursor is not hidden, and the viewport is not locked. Keyboard and wheel camera actions remain available.

### Screenshot existence can be mistaken for visual approval

The visual workflow requires an equivalent state, a side-by-side board, a checklist, three prioritized mismatches, intentional-difference notes, and a bounded recapture. Automated black-image and scale checks remain necessary but cannot approve visual fidelity.

### Pixel matching can override usability or gameplay truth

Concept art remains directional. Written accessibility, 2D, information, and simulation requirements override incidental geometry, text, and values.

### Generated art can silently become unreviewed production content

Local ComfyUI and OpenAI generation produce candidates only. Every retained source records generator/workflow, prompt, seed or request identity when available, dimensions, output hash, edits, intended use, and review decision. Generated candidates do not bypass CT-01 or other provenance gates.

### Deferring the soak can hide accumulating performance debt

Each phase still runs short deterministic performance, memory, save-latency, backlog, and packaged smoke checks. The four-hour physical-tier soak is deferred, not deleted, and returns as a release-hardening gate.
