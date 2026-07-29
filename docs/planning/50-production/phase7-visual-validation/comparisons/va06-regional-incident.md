# VA-06 Visual Comparison — Weather and Incident

**Equivalent state:** Regional runway incident after area protection, with
other airport operations continuing  
**Board:** [actual versus concept](va06-regional-incident.png)

## Review

- Weather, wind, visibility, forecast confidence, localized runway closure,
  response progression, continuing-operations status, cause, remedy, recovery
  control, and airport radio caption are visible.
- Coral hatching, boundary markers, response connectors, directional arrows,
  vehicles, and text labels make the affected area color-independent.
- The large world view and two concise rails closely follow the concept's
  incident hierarchy.

## Highest-impact differences

1. The concept has a darker rain layer, wet reflections, denser apron activity,
   emergency lighting, and stronger atmospheric contrast.
2. The implementation uses brighter project-authored facility sprites and more
   diagrammatic response routes.
3. The concept presents a larger action tray; the implementation uses a
   bounded cause/remedy panel and one feasible recovery action.

## Intentional differences

- Only the affected runway is closed. Other operations remain visibly active,
  matching simulation truth rather than dramatizing a full-airport shutdown.
- The exact weather, cause, acknowledged decision, and remedy are retained
  incident records.

## Bounded correction completed

The incident capture now waits until the protected state, and the regional
navigation stays on Incident until the player explicitly changes surface.
This exposes the complete cause/consequence/recovery chain in one image.
