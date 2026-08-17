# Continuous terminal cutaway preliminary adversarial review

Date: 2026-08-16
Status: Approved with controls below

## Failure cases and controls

1. **The airport shell remains visible but exterior world proxies still
   disappear.** Assert procedural facilities, infrastructure, landscaping, and
   bounded mature sprites remain active during cutaway.
2. **The root and terminal overlays both draw headers or time controls.** Hide
   the terminal replacement background, top bar, footer legend, save/load, and
   screen-fixed world labels; retain only contextual terminal tools.
3. **A transparent full-screen widget blocks ordinary airport controls.** Use a
   bounded map-interaction surface and self-hit-test-invisible outer overlay.
4. **Terminal placement stops working after labels are removed.** Derive
   gestures from the bounded interaction surface and deproject to the existing
   schema-10 world grid.
5. **The camera still behaves like a separate scene.** Preserve the normal map
   bounds and full zoom-out range; restore the exact prior frame on close.
6. **Nearby airport activity renders over terminal furnishings incorrectly.**
   Retain the existing world-layer contract: roads and pads below terminal
   floors; terminal walls, objects, workers, visitors, and overlays above.
7. **The legacy Phase 3 proof renderer returns with the exterior.** Keep mature
   overview authoritative and assert zero fixed-coordinate legacy proxies.
8. **Incident presentation is lost while inspecting a terminal.** Preserve
   ordinary mature-world incident tint, closures, and response proxies unless
   terminal-local readability requires a separately reviewed exception.
9. **Close Cutaway loses the player's map position.** Store management location
   and ortho width only on the first focus transition and restore them exactly.
10. **The change mutates gameplay or saves.** Do not modify simulation commands,
    state, schema, balance, migration, economy, routing, or scheduling.

## Required proof

- Focused presenter and camera automation followed by the complete AMSim suite.
- Equivalent-state packaged captures at overview, cutaway, and restored
  overview.
- Clean Development and Shipping packaging, runtime smoke, and zero forbidden
  editor/MCP/runtime-string-load matches.
