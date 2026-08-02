# Construction Surface Layering Adversarial Review

**Date:** 2026-08-02
**Scope:** Phase 1 unfinished construction beds and revealed movement surfaces

## Failure modes reviewed

1. **Changing only translucent priority would leave masked sprites coplanar.**
   Rejected. Every construction bed now uses both a lower physical Z plane and
   a lower translucent sort priority than every runway, taxiway, and road
   surface.
2. **Per-segment placeholder priorities could rise above another completed
   surface at an intersection.** Rejected. All placeholder beds share layer 5;
   every movement surface begins at layer 10 or higher.
3. **The bed could remain visible after completion and leak around transparent
   texture edges.** Existing lifecycle mapping hides all earthwork beds at
   inspection and later. Regression coverage now asserts this transition.
4. **The presentation fix could affect construction progress or persistence.**
   It changes component depth/sort metadata only. No simulation, query, schema,
   asset, balance, or save behavior changed.

## Verdict

Accepted. The renderer no longer depends on undefined coplanar depth behavior,
and the invariant is covered at purchase, partial reveal, and completion.
