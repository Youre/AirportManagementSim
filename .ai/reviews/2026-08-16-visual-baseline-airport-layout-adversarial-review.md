# Visual-baseline airport layout preliminary adversarial review

Date: 2026-08-16
Status: Approved with controls below

## Failure cases and controls

1. **Only the save is replaced and the overlap remains.** Centralize and test
   the presenter-owned exterior layout before refreshing the deterministic save.
2. **The terminal is made smaller to create space.** Preserve the approved
   sixteen-meter module, grown footprint, furniture scale, and identity scale.
3. **Gate access is separated from the terminal.** Gate pads are the only
   allowed bridge across the terminal-to-apron clearance and remain aligned to
   the airside edge.
4. **Landside facilities move off-screen.** Keep every fixture footprint inside
   the shared management-camera review area and validate through the packaged
   1920x1080 capture.
5. **Ambient sprites still overlap the interior.** Relocate mature vehicles,
   people, landscaping, selection, and apron fixtures alongside the procedural
   surfaces, not only the large facility meshes.
6. **Incident overlays point at the old runway.** Move the affected aircraft,
   closure, response route, and protection zone with the new runway/service
   geometry and rerun the Phase 4 deterministic journey.
7. **A visual-only layout starts mutating simulation.** Keep all new coordinates
   in the presentation module; do not write positions into Phase 1-4 state or
   change deterministic checksums.
8. **The refresh destroys an owner save.** Target only `VisualBaseline`, require
   `-Refresh`, retain the automatic backup, and compare all other save files
   before and after installation.
9. **The comparison hides the original problem.** Capture the same slot,
   viewport, UI scale, overview mode, and management-camera framing used by the
   overlapping implementation record.
10. **The correction reintroduces mixed-perspective art.** Continue using
    procedural strict-nadir infrastructure and the existing bounded top-down
    identity sprites.

## Acceptance condition

Accept only when the packaged overview reads in the VA-01 order—runway,
taxiway, apron, terminal, landside access—with clear zone boundaries and no
unintended terminal overlap, while the ordinary Phase 4 fixture, save/load,
single-canvas terminal, and complete automation remain intact.
