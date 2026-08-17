# Terminal double-scale adversarial review

Date: 2026-08-16
Status: Passed with packaged visual gate required

## Failure modes

1. **Only the floor doubles while click targets stay small.** Prevented by the
   shared renderer/deprojection geometry constants and focused mapping test.
2. **Furniture becomes relatively microscopic.** Prevented by doubling the
   bounded content-readability scale with the facility module.
3. **The terminal falls behind the footer or outside the overview.** The fixed
   campus moves north into the mature landside/apron composition; packaged
   1920x1080 inspection is mandatory.
4. **The terminal covers the landside access road.** The doubled south edge is
   positioned immediately north of that road. The first packaged frame must be
   rejected if road markings cross the floor.
5. **The terminal becomes a separate mode or camera target.** Camera and tool
   code are out of scope; existing same-frame assertions remain mandatory.
6. **Overlapping mature facility geometry flickers through the floor.** The
   layer contract keeps roads/facilities below terminal floors, but packaged
   inspection must verify edges and details. Relocate bounded campus props only
   if a visible collision remains.
7. **An old save changes simulation outcomes.** No simulation or serialized
   field changes; full deterministic automation remains required.

## Gate

Accept only a packaged equivalent-state capture where the entire grown
terminal reads as a primary airport facility, stays visible in the continuous
world frame, clears the access road, and preserves the single-camera contract.
