# Unified build-world preliminary adversarial review

Date: 2026-08-16
Status: Approved with controls below

## Failure cases and controls

1. **Preview mutates simulation.** The presenter receives a copied proposal and
   visual flags only. It cannot submit commands or write proxy positions back.
2. **Preview overwrites committed geometry.** Use dedicated pooled preview
   components. The committed runway, taxiways, road, terminal, and gates remain
   driven only by revisioned simulation snapshots.
3. **Confirm causes a visual jump.** Preview and committed components must call
   the same `MakePhase1WorldSegmentGeometry` transform with identical widths.
4. **A cancelled plan remains visible.** Closing, resetting, confirming, and
   destroying the view must clear every preview proxy.
5. **Build mode restores the old terminal.** Never draw or reveal the legacy
   `OperationsHut` when schema-10 terminal cells exist. Build-mode visibility
   transitions must preserve the generated roof/cutaway state.
6. **Fixed terminal or gates disappear before a plan exists.** Starter context
   remains visible when the airport is initialized even if the movement
   network is not committed.
7. **Pointer placement drifts after pan or zoom.** Convert the Slate pointer to
   viewport coordinates, deproject through the active camera, intersect the
   airport plane, and invert the shared parcel transform.
8. **UI chrome receives world clicks.** Retain explicit world-region hit
   testing before deprojection and keep tool/action panels above the input
   surface.
9. **Right-click panning regresses.** Build mode must not consume non-left
   pointer buttons. The root camera-pan path must remain available while the
   proposal is open.
10. **Snap indicators no longer align.** World-space snap/endpoint overlays must
    derive from the same proposal points as validation. Any temporary retained
    screen-space labels may describe state but may not impersonate geometry.
11. **Preview leaks components.** Allocate the bounded runway/taxiway/road and
    marker pool once in the presenter constructor and toggle visibility.
12. **Uninitialized CDO tests dereference world state.** Public test helpers
    must report static pool/assets safely; runtime preview tests must use a real
    automation world and spawned presenter.
13. **Packaging omits an asset.** Preview reuses constructor-visible PaperSprite
    references already required by the committed world; no runtime string load
    is added.
14. **Broad refactor destabilizes saves or construction.** Do not change Phase 1
    commands, validation, costs, stage timing, workers, or schema 10 records.

## Required proof

Focused unit/presentation automation, the full project automation suite,
Development and Shipping packaging, cooked-asset and forbidden-dependency
scans, and a same-state visual comparison are required before closeout.
