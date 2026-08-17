# Procedural overview recovery preliminary adversarial review

Date: 2026-08-16
Status: Approved with controls below

## Failure cases and controls

1. **A loaded save restores simulation but leaks the previous UI destination.**
   Close every full-screen destination and explicitly restore mature overview
   mode after load and Back.
2. **The old Phase 3 interior reappears behind another root-level surface.**
   Assert that its floors, rooms, routes, props, passengers, bags, staff,
   vehicles, and aircraft are all hidden whenever overview mode is active.
3. **The schema-10 terminal is hidden with the legacy renderer.** Keep the
   generated roof visible in overview and the spatial cutaway visible after the
   Terminal action; do not reuse the legacy fixed-coordinate arrays.
4. **Procedural facilities overlap roads or gates.** Use the existing world
   presentation layers, authored footprint bounds, and the same overview
   coordinate frame as the procedural infrastructure.
5. **Procedural replacements become featureless rectangles.** Encode border,
   roof/pad seams, entrances, parking/transport markings, and functional
   identity with geometry and redundant color/shape treatments.
6. **Retired sprites remain visible beneath the new meshes and flicker.** Hide
   every superseded mature-site sprite and count active legacy proxies in
   automation.
7. **Incident tint applies to only half the exterior.** Include procedural
   facilities in the mature infrastructure tint family.
8. **Meshes exist in Editor but fail in packaged builds.** Retain the existing
   constructor-visible material, test vertex/triangle counts and material
   presence, and exercise the actual packaged load journey.
9. **The correction changes saved gameplay.** Do not alter simulation state,
   schema, migration, balance, or command handling.
10. **The airport loses visual identity by proceduralizing everything.** Keep
    aircraft, workers, service vehicles, fixtures, and reviewed landscaping as
    bounded sprites; replace only scale-sensitive surfaces and footprints.

## Required proof

- Focused presentation automation followed by the complete AMSim suite.
- Equivalent-state packaged captures for load overview, Terminal cutaway, and
  Back-to-overview.
- Clean Development and Shipping packaging, runtime smoke, and zero forbidden
  editor/MCP/runtime-string-load matches.
