# Terminal normal-scale architecture impact

Date: 2026-08-16
Status: Approved for implementation

## Change

Enlarge the schema-10 terminal on the authoritative airport canvas without
changing simulation identity, persistence, camera ownership, or renderer
ownership. A logical terminal cell remains the saved/editable unit, while its
world presentation uses one shared eight-meter module. This makes the furnished
starter terminal approximately 144 by 96 meters and the grown regional terminal
approximately 240 by 144 meters. The value is a gameplay-readable composition
module, not a claim that every logical construction cell is an eight-meter room.

## Boundaries

- `AAMSimWorldPresenter` remains the only terminal world renderer.
- The terminal remains permanently roofless and uses one fixed landside-campus
  anchor derived from the existing map anchor plus a presentation-only offset.
- Terminal tools remain contextual chrome and never move the camera.
- Simulation records, schema 10 serialization, costs, routes, and stable IDs do
  not change.
- Pointer-to-cell conversion and proxy placement consume the same exported
  presentation geometry constants; no second coordinate model is introduced.
- Furniture, people, bags, vehicles, construction workers, and overlays use a
  bounded content-readability scale rather than inheriting the full building
  footprint multiplier.

## Affected modules

- `AMSimTerminalPresentationGeometry.h`: shared presentation dimensions.
- `AMSimWorldPresenterTerminalLayout.cpp`: terminal proxy placement and scale.
- `AMSimTerminalView.cpp`: pointer mapping and physical-size feedback.
- `AMSimPresentationTests.cpp`: normal-size footprint, tool continuity, and
  coordinate-alignment regression coverage.

## Compatibility

This is presentation-only. Existing schema 1-10 saves load without migration,
and deterministic simulation checksums remain unchanged. Cook dependencies are
unchanged because no new runtime asset is required.

## Final bounded adjustment

Packaged inspection showed that the enlarged footprint crossed the access
road. The shared campus center now applies a -6,000 world-X-unit offset (60
presented meters), placing the terminal immediately south of the road. Render
placement and pointer mapping consume the same center, so this does not create
a second coordinate path.
