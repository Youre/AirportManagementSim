# Phase 1.5 Pre-Implementation Adversarial Review

**Status:** Risks bounded; implementation may proceed  
**Date:** 2026-07-26

| Risk | Failure mode | Required mitigation |
| --- | --- | --- |
| Blueprint cooker omission | Shipping silently uses native fallback | Hard class reference, cook audit, container presence check, packaged interaction proof |
| Presentation becomes authoritative | Actor/widget state changes outcomes | Snapshot-only input, typed commands only, no presenter callbacks into simulation |
| Per-frame UI cost | Full string/layout rebuild every tick | Revision-gated view-state application and bounded clock refresh |
| Paper2D scale mismatch | Airfield is unreadable or physically misleading | 32 px/m source contract, definition-based aircraft dimensions, orthographic camera fixture |
| Cursor regression | Click capture hides or locks the Windows cursor | Preserve CommonUI combined input, click-only capture, no lock, manual packaged observation |
| High UI scale hides world | 175–200% recreates the text-wall prototype | Compact rail mode and deterministic 100–200% captures |
| Generated texture inconsistency | Photorealistic or visibly tiled surfaces | Low-opacity normalized candidates, seam/contact-sheet review, flat-color fallback |
| External aircraft uncertainty | Unapproved art enters the package | One-row manifest, owner reuse approval, exact checksum, no additional external imports |
| Real branding leaks | Source aircraft implies a real operator | Fictional yellow Riverbend livery and fictional in-game identity; no operator logos |
| MCP leaks into runtime | Shipping opens a development endpoint | Editor-only plugins/toolsets and forbidden dependency scan |
| Visual similarity masks behavior loss | Polished controls stop working | Re-run the complete Phase 1 command journey, save/load, rendered smoke, and automation |

The implementation must stop and fall back to reviewed flat presentation assets if generated candidates fail seam, readability, or package review.

