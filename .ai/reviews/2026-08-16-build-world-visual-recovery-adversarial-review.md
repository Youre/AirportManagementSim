# Build-world visual recovery preliminary adversarial review

Date: 2026-08-16
Status: Approved with controls below

## Failure cases and controls

1. **The visual recovery reintroduces the duplicate UMG airport.** All map
   geometry stays in `AAMSimWorldPresenter`; UMG remains tool and validation
   chrome.
2. **Road/taxi preview covers a gate or terminal.** Base surfaces, outlines,
   and dashes sort below apron/gate and terminal layers. Only compact markers
   sort above them.
3. **Taxiway disappears at a runway crossing.** Taxiway remains above runway
   within the movement-surface band, while both remain below structures.
4. **The planning grid competes with the airport.** Grid lines are thin,
   low-alpha, and below every facility; the parcel boundary is stronger but
   still below placement geometry.
5. **Markers become enormous when zooming.** Marker scale derives from the
   active orthographic width and is clamped to the supported management zoom
   band.
6. **Markers become unreadable at default zoom.** Reviewed snap/connection and
   blocked sprites replace the undersized generic ring where semantic state is
   needed; endpoint circles retain a minimum management-zoom size.
7. **Color is the only proposal state.** Outline, center dashes, ring/crosshair,
   check/connection, and blocked-X silhouettes accompany cyan/coral color.
8. **An invalid segment paints the whole network coral.** Preview state carries
   per-surface styling; diagnostics mark the affected surface and point.
9. **Pooling regresses performance.** Grid, outline, dash, and marker components
   are allocated once to bounded maxima and hidden/reused between refreshes.
10. **A cancelled or closed proposal leaks overlays.** Clear hides base,
    outline, dash, grid/boundary, marker, and diagnostic proxies.
11. **Presentation changes simulation.** Preview state is copied from the
    proposal and remains write-only to transient sprite components.
12. **Cooking drops marker assets.** Fixed constructor-visible references are
    added to the required-presentation audit and exercised in packaged build
    mode.

## Required proof

Focused and full automation, UE 5.8 Editor/Game builds, real same-state visual
inspection, clean Development packaging, packaged smoke, cooker/dependency
scan, and documented VA-02 mismatch review are required before closeout.
