# PI-10: Phase 7 Content, Accessibility, and Release Hardening

**Status:** Implementation active; full owner acceptance deferred until the
release candidate is ready  
**Owner:** Product, presentation, content, quality, and release engineering  
**Approved:** 2026-07-28  
**Normative requirements:** PX-01 through PX-03, CT-01 through CT-05,
TS-01 through TS-09, PI-01, PI-02

## Outcome

Phase 7 turns the technically complete Phase 1-6 game into one identifiable,
auditable offline Windows release candidate. It adds no new airport simulation
mechanics. It completes in-product guidance and accessibility controls,
centralizes captions-first offline speech, locks the curated content and rights
inventory, proves every supported save migration, and produces reproducible
build, security, visual, and human-test evidence.

The owner has elected to run the consolidated full test after Phase 7
implementation. The four-hour reference-tier soak, genuine pacing journey,
six Phase 5 Advanced journeys, six Phase 6 Major journeys, final visual
checkpoint, and formal unassisted tester protocol therefore remain the final
acceptance actions. They are not represented as passed by automation.

## Player-visible implementation

### Contextual guidance

- First-use help appears as a short, anchored, dismissible card beside the
  applicable gameplay state.
- Starter guidance covers airport naming, construction, opening, offer
  compatibility, scheduling, and watching the first turnaround.
- Acknowledged help is stored in the airport save, not the global profile.
- Every dismissed starter item remains available in the Help surface.
- Help copy remains at or below 45 words per card and pairs authentic terms
  with a plain explanation and one feasible next action.

### Help and accessibility surface

Add one production Help surface with:

- `START`: the complete first-airfield journey without forcing a tutorial;
- `CONTROLS`: semantic mouse/keyboard actions, time, selection, focus, and
  visible-cursor behavior;
- `AIRPORT`: short explanations of runway, taxiway, stand, gate, turnaround,
  compatibility, and capability;
- `OPERATIONS`: scheduling, automatic dispatch, weather, incidents, cargo,
  passengers, and shared resources;
- `ACCESSIBILITY`: application scale at 100%, 125%, 150%, 175%, and 200%,
  captions-on status, reduced-motion preference, camera-shake-off status, and
  reset-to-defaults.

Application scale is a local profile setting and never carries airport
progression. Changing it reapplies the production shell so compact drawers
activate at 175-200%.

The surface reuses the Riverbend template: rounded navy chrome, cyan selection,
amber caution, coral critical, green confirmation, off-white type, concise
cards, redundant icon/pattern/text state, and no implementation identifiers.
VA-01 supplies the world/HUD relationship, VA-06 the caption and
cause/consequence language, and VA-07 the approachable progress language.

### Offline radio and captions

- One game-instance service owns the local speech provider and fallback cue.
- Phase 1 structured phrase intents and later operational captions use the
  same deduplicated captions-first path.
- Captions remain immediate and authoritative if local speech is unavailable.
- The release phrase catalog covers the approved ATIS, arrival, pattern,
  landing/takeoff, hold-short, crossing, taxi, stand, go-around, dispatch,
  boarding, passenger-page, weather-warning, and emergency families.
- No runtime phrase, voice sample, telemetry, or player data leaves the
  machine.

## Technical contracts

- Increment snapshots to schema 8 solely for the bounded set of acknowledged
  contextual-help IDs. Schemas 1-7 migrate with an empty acknowledged set.
- Help acknowledgement is a typed Phase 1 command/event and contributes to the
  deterministic checksum. It cannot change economy, AP, construction,
  scheduling, services, weather, incidents, or capability evidence.
- Store cross-slot accessibility settings in a config-backed local profile.
  The profile contains no save progress, names, or personal data.
- Add `/Game/Phase7/Definitions` Primary Assets for the release map/catalog
  lock, guidance, accessibility, radio coverage, rights manifest, release
  manifest, and retained test protocols.
- Add focused Phase 7 code and tests. Do not extend the near-limit root,
  regional, world-presenter, or serializer files beyond 2,000 lines.
- Required release content uses Primary Asset or constructor-visible
  references. Runtime string asset loading remains prohibited.

## Curated content and release identity

The first release contains the approved `Map.TemperateStarter` region, which
supports all six specializations. The release catalog includes only the
aircraft, fictional operators, facilities, events, and presentation assets
already approved through Phases 1-6. It does not import another
`Joes_Game` aircraft, any military aircraft, real operator branding, 3D
content, or unreviewed music.

The release manifest records:

- engine, platform, schema, map, rules, and content versions;
- source commit and content hashes;
- approved aircraft/operator and third-party notice inventories;
- rights/provenance status;
- required test scenarios and human protocols;
- Development and Shipping executable hashes;
- network, dependency, cook, migration, and visual-audit results.

User-owned, unreviewed files under `SourceAssets/Audio/Music` remain outside
the release candidate until a separate provenance and mix review approves
specific tracks.

## Visual validation

1. Re-run the deterministic Unreal capture journey at 1920x1080.
2. Compare actual implementation captures—not illustrative HTML boards—to
   VA-01 through VA-07 at equivalent states.
3. Capture the Help/accessibility surface at all five scales.
4. For every state, record the three highest-impact differences, written-spec
   overrides, and one bounded correction where needed.
5. Inspect the final images for hierarchy, world area, rounded component
   language, copy density, status redundancy, cursor visibility, captions, and
   compact-mode behavior.

Concept art remains directional. Written gameplay, accessibility, child-safety,
2D, and information requirements override incidental generated details.

## Automated and packaged acceptance

- Phase 7 focused tests cover help selection, acknowledgement, duplicate
  rejection, schema-8 round-trip, schemas 1-7 migration, profile clamping,
  phrase coverage, caption deduplication, content validation, and release
  manifest integrity.
- The complete Phase 1-7 automation suite passes with no failures or not-run
  tests.
- Deterministic Phase 1 and Phase 4 rendered fixtures still pass and produce
  current actual screenshots.
- Clean Development and Shipping packages build from the recorded source
  identity.
- Shipping contains every required Phase 7 asset and the production root
  Widget Blueprint.
- Static and package scans report zero editor/MCP/test/tool dependencies,
  runtime string loads, required 3D assets, secrets, credentials, unexpected
  endpoints, unapproved aircraft, or real operator branding.
- Offline packaged smoke records zero unexpected TCP connections.
- Every retained schema fixture migrates to schema 8 and deterministic
  continuation remains stable.
- Source and header files remain at or below 2,000 lines.

## Final owner acceptance

After technical implementation, the owner runs the consolidated full test
against the exact candidate hashes:

1. six Phase 5 Advanced prepared journeys;
2. six Phase 6 Major prepared journeys;
3. the 15-minute maximum-load observation;
4. one genuine 10-15 hour pacing journey;
5. the four-hour physical reference-tier soak;
6. the formal unassisted tester, audio, caption, save/load, and comprehension
   protocol;
7. the final concept-art visual checkpoint.

Phase 7 closes only when those records pass with zero release blockers. Until
then, the correct status is `release candidate ready for owner acceptance`,
not `complete`.

## Rollback

The release UI, profile, radio service, Phase 7 data assets, and audit scripts
can be removed without changing Phase 1-6 simulation behavior. A schema-8 save
cannot be downgraded in place; the prior schema-7 file remains protected by
the existing current/backup/recovery policy.
