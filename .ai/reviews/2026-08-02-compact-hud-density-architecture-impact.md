# Compact HUD Density Architecture Impact

Date: 2026-08-02
Status: Approved for implementation

## User-visible outcome

The permanent left menu becomes a narrow icon rail. Hovering an icon or moving
keyboard focus to it reveals its label in a flyout without resizing the world.
Active starter-airfield construction becomes one concise two-line card in the
upper-right map corner rather than a wide title plus two nested evidence boxes.

## Boundaries

- Simulation commands, query mapping, save data, construction balance, world
  presentation, input policy, and audio semantics remain unchanged.
- The root presenter continues to bind the same command handlers. A focused
  reusable widget owns icon, label, hover/focus expansion, tooltip, and shared
  button styling.
- Existing project-owned, cooker-visible Phase 4.5 UI textures are reused
  through constructor-time hard references. No runtime string loading or new
  generated asset enters the project.
- The near-limit root-screen implementation loses the old verbose rail
  construction. New reusable behavior lives in focused source files.

## Layout and accessibility

- The rail overlays the left map edge at 112 units and does not reserve the old
  190-unit column.
- Each icon retains an 88-by-56 hit target. Flyout labels are visual and
  hit-test-inert, so click behavior remains on the stable icon target.
- Hover and keyboard focus reveal the same label; tooltips and accessible text
  preserve meaning when an icon is unfamiliar.
- Construction status uses explicit stage text plus runway/cost text. Color is
  supplementary.

## Migration and rollback

There is no schema, content, or persistence migration. Rollback restores the
old fixed-width left column and wide context-card composition. Existing command
bindings and runtime assets remain valid either way.
