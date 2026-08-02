# Construction Progress Feedback Architecture Impact

Date: 2026-08-02
Status: Approved for implementation

## User-visible outcome

Confirming and purchasing a valid starter-airfield proposal immediately
dispatches the construction truck and crew. They travel from the starter
terminal or useful service-road approach to the accepted geometry, work along
that geometry, and leave only after inspection. Full-length graded-earth beds
appear first; the approved runway, taxiway, and road surfaces then reveal
progressively and become the final textures before opening.

## Affected contracts

- Phase 1 remains the authority for the project stage, funded timestamp,
  delivery state, proposal geometry, road travel benefit, and simulation time.
- No new save field or schema migration is required. Delivery, building, and
  inspection timing continue to derive from the existing deterministic
  fixture thresholds and survive save/load through existing project state.
- `AMSimUI` adds a pure construction-presentation mapper. It converts immutable
  Phase 1 state into travel and surface progress without mutating simulation.
- The Paper2D presenter adds bounded, constructor-created pools for earthwork
  beds and a small construction crew. It remains revision-gated and has no
  actor tick.
- Finished-surface reveal advances proportionally across the accepted runway,
  every taxiway branch, and the optional road so the distributed crew visibly
  works on all components together. The stored proposal is never altered.

## Visual and asset boundary

- Existing cooker-visible project sprites remain the runtime source for the
  finished grass runway, taxi wear, and service road.
- The earthwork bed reuses the existing taxi-wear sprite with a distinct
  brown, pattern-backed treatment. It therefore needs no new runtime asset,
  string load, cook rule, or migration.
- A local ComfyUI graded-earth candidate pass was reviewed because simple
  surface textures are an approved local-generator use. The candidates contain
  strong baked rectangular roller patterns and are not retained as runtime
  assets; the reusable tint treatment is clearer at the current world scale.
- VA-02 remains the visual reference: visible delivery/crew activity, a
  protected work area, clear proposed-versus-finished state, and a map-dominant
  composition.

## Rollback and verification

The change can be rolled back by removing only the presentation mapper and
new proxy pools; authoritative saves and simulation outcomes remain compatible.
Verification must cover threshold mapping, road-shortened timing, monotonic
length allocation, immediate dispatch visibility, worker travel, stage-based
surface replacement, no per-frame tick, save/load continuity, and the existing
Phase 1 and presentation suites.
