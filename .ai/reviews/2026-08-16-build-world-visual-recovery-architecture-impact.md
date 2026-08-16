# Build-world visual recovery architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

- Build mode keeps the unified Paper2D airport world while restoring the
  proposal language required by VA-02: a subdued planning grid and parcel
  boundary, cyan patterned proposal geometry, compact endpoint circles, clear
  snap/connection nodes, and coral localized conflicts.
- Road, runway, taxiway, apron/gates, buildings, terminal roof, preview
  overlays, and markers use an explicit depth order. Movement and road surfaces
  cannot paint over gate or terminal artwork; interaction markers remain above
  the structures they identify.
- Confirmation continues to retain exact geometry and changes only state and
  treatment.

## Current regression

- The first unified preview pass reused finished movement-surface sprites with
  translucent tint but did not rebuild the former proposal outline/pattern,
  parcel grid, or fixed-screen-size connection-node language.
- Preview runway, taxiway, and road components were assigned sort priorities
  86-94 and heights 84-92, above the starter gates and generated terminal.
- Every marker reused the small Phase 1 selection ring at sub-unit scale, which
  is not readable at the 105,000-unit management camera width.

## Target boundary

- `AAMSimWorldPresenter` remains the only airport-world renderer.
- `UAMSimConstructionProposalView` continues to own pointer gestures,
  authoritative validation mapping, and immutable preview-state creation.
- A shared Phase 1 world-layer contract owns both height and translucent sort
  priority for terrain, road, runway, taxiway, apron/gate, structure, proposal
  pattern, and marker layers.
- Proposal surfaces remain below apron/gate and terminal layers. Pooled outline
  and dash components provide non-color selection/pattern evidence below
  structures. Pooled marker components use reviewed cooker-visible placement
  sprites above structures.
- The planning grid and parcel boundary are pooled world geometry visible only
  in build mode. They are presentation-only and never become collision,
  simulation geometry, or save state.

## Asset decision

- Reuse the project-authored Phase 1 white-square/selection-ring sprites and
  reviewed TerminalGrowth placement sprites for endpoint, snap, connected, and
  blocked states.
- No generated or external asset is required. No runtime string load is added;
  fixed sprites use constructor-visible references.

## Compatibility and rollback

- Schemas 1-10, simulation commands, costs, snapping, construction timing, and
  saved proposal geometry are unchanged.
- The change is isolated to pooled presentation components, preview-state
  styling, and depth constants. It can be reverted without migration.

## Required proof

- Focused automation proves the grid/boundary and pattern pools activate and
  clear, required marker sprites are loaded, base/overlay surfaces remain below
  gates and terminal context, markers remain above, and pool allocation is
  bounded and reused.
- An equivalent 1280x720 build-mode replay and a VA-02 comparison board prove
  visible handles, spatial conflict evidence, proposed-versus-built contrast,
  gate/terminal legibility, and no preview geometry jumping on confirmation.
