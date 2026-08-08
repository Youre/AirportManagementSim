<!-- ai-project-init:begin managed -->
# Decision Log

- Initial AI engineering OS scaffold generated.
<!-- ai-project-init:end managed -->

## Project decisions

- 2026-07-26: The approved concept art's required visible content is promoted into CT-05 with stable requirement IDs. The accepted Riverbend palette/component shell remains, but responsive, package, and semantic passes no longer count as concept-content parity while required world objects, task-specific visualizations, or equivalent states are absent. Phase 4.5 closes this cross-phase presentation debt before Phase 5 opens.
- 2026-07-27: Phase 4.5 is accepted as the technical and visual-content baseline
  with continuing aesthetic polish debt. Phase 5 makes per-path capability
  evidence authoritative and non-exclusive after initialization, retains the
  Phase 2 selected-specialization field only for schema compatibility, keeps
  Phase 1 as economy/AP authority, and defers Major/wide-body capability and
  long-form human gates to Phases 6 and 7.
- 2026-07-28: Phase 6 uses the owner-approved River & Sun treatment for the
  fictional Riverbend Longreach 787-9. Approval covers only the recorded
  Boeing 787-9 reference checksum and a project-authored top-down vector
  master with 16 headings; it does not approve other `Joes_Game` aircraft or
  real airline branding. Phase 6 technical implementation may close while
  owner journeys, final visual approval, and the bounded 15-minute
  maximum-load observation remain explicit acceptance gates.
- 2026-07-25: Unreal Engine replaces the Godot reference in `ideas.txt` as the target engine.
- 2026-07-25: The game is 2D, cartoon-styled, and top-down; 3D assets and 3D presentation are excluded.
- 2026-07-25: The original aircraft-art library was a read-only source of candidates until an import and rights review was approved.
- 2026-07-25: On the replacement workstation, the read-only candidate aircraft source is `C:\Users\dave\Documents\Joes_Game\dist\assets`; its build-hashed filenames do not bypass CT-01 provenance, rights, or import-manifest approval.
- 2026-07-25: The concept-definition-only stage is superseded by the approved implementation-planning set rooted at `docs/planning/00-foundation/specification-index.md`.
- 2026-07-25: Runtime simulation uses deterministic lightweight C++ records with stable IDs; Actors, sprites, widgets, and audio are presentation only.
- 2026-07-25: The rendering baseline is an orthographic Paper 2D world with CommonUI/UMG; the product remains 2D-only.
- 2026-07-25: Saves use versioned asynchronous snapshots, migrations, atomic replacement, and previous-known-good recovery.
- 2026-07-25: The initial product is offline single-player Windows desktop with mouse and keyboard; it has no gameplay backend, account, analytics, cloud, or multiplayer dependency.
- 2026-07-25: Unreal MCP and related automation are localhost editor-only tools and are excluded from Shipping builds.
- 2026-07-25: The project disables engine plugins by default and explicitly opts in to runtime capabilities; editor automation and developer tests are target-scoped and must remain absent from Shipping.
- 2026-07-25: The CORSAIR VENGEANCE i5200/RTX 5090 workstation is recorded as the Phase 0 measurement host, not the minimum or release-reference PC. A representative reference tier must be selected before Phase 1 closes.
- 2026-07-25: All six airport specializations are viable endgames; passenger international-hub growth is optional rather than the universal success path.
- 2026-07-25: The seven-image concept-art set under `docs/planning/30-content-and-assets/concept-art/` is approved supporting evidence for visual comparison gates. Written specifications remain authoritative; generated values, geometry, typography, dimensional shading, and incidental details are non-normative.
- 2026-07-26: Phase 1 is organized around one stable `S01 Starter grass airfield` journey and work packages P1-00 through P1-08; each package must extend that journey rather than land a disconnected system.
- 2026-07-26: `Aircraft.LightPiston.Starter` is the stable Phase 1 aircraft content ID. Development defaults to internally authored, provenance-recorded 2D art; no external candidate sprite is copied without a completed CT-01 manifest and review.
- 2026-07-26: Phase 1 captions are the immediate authoritative meaning of radio phrase intents. A local speech provider is optional behind a replaceable contract and must fall back to captions plus a local cue without affecting simulation or requiring a network.
- 2026-07-26: The Phase 1 starter offer appears immediately when the airfield becomes ready and planning remains paused; the first aircraft must arrive within three game minutes after schedule confirmation. This resolves the prior conflict between a three-minute post-opening target and five-minute timetable slots without adding a special-case clock or timetable rule.
- 2026-07-26: Phase 1 implementation uses an internally authored vector 2D airframe marker and local Flite TextToSpeech behind captions-first fallback. No candidate aircraft file was copied; stable content ID and save contracts remain presentation-independent.
- 2026-07-26: Phase 1 implementation may be marked verified on the development host, but formal closure remains blocked until elevated firewall-denied S15, physical reference-tier measurements, and an unassisted tester/audio/comprehension review pass.
- 2026-07-26: Elevated Phase 1 S15 passed against the final package hashes with exact-executable inbound/outbound denial, Development journey/save-load continuity, zero Shipping TCP sockets, and complete rule cleanup. Formal closure now depends only on physical reference-tier and unassisted tester/audio/comprehension evidence.
- 2026-07-26: The four-hour physical-tier soak and formal unassisted tester/audio/comprehension record are deferred to Phase 7 release hardening. Phase 1 continues short automated and hands-on checks and remains focused on visual fidelity and interaction polish rather than blocking subsequent development on premature endurance evidence.
- 2026-07-26: Visual validation requires equivalent-state side-by-side comparison, prioritized mismatch notes, a bounded mutation, and same-state recapture. OpenAI Image Gen is preferred for concept/UI exploration; local ComfyUI is approved for simple textures, variants, edits, and cutouts subject to full provenance and content review.
- 2026-08-02: The starter map now supplies a basic operations-terminal shell
  with two preset aircraft gate/stand ports so construction has visible
  context. Players draw every runway and a multi-segment taxiway graph;
  readiness requires the whole proposed graph to connect a runway to at least
  one gate. Optional service roads never require a stand connection and only
  accelerate nearby construction travel. Full passenger-terminal mechanics
  remain Phase 3.
- 2026-08-02: The complete 28-cue ElevenLabs UI sound inventory is approved
  for nonverbal UI-only use. Source MP3 masters remain outside cooked content;
  typed hard references and `/Game/Audio/UI` provide runtime/cooker ownership.
  All future Riverbend buttons inherit hover/press audio from the shared theme,
  while semantic cues are emitted only from authoritative results or state
  transitions and never replace visible/captioned meaning.
- 2026-08-08: PI-11 replaces the static starter terminal with a growable,
  ground-floor, one-meter-grid terminal inside the airport world. The first
  rollout uses a furnished GA seed, hybrid immediate-spend editing, fully
  refundable unfinished work, local construction closures, ordinary workers,
  a generated roof/cutaway transition, schema-10 persistence, and a spatial
  bridge for the existing Phase 3 fixture. Upper floors and player-placeable
  passenger-processing modules remain explicit later work.
- 2026-07-26: Phase 1.5 establishes the reusable presentation baseline with a cooker-visible production Widget Blueprint, revision-gated native presenter, real Paper2D world, one approved fictional-livery Cessna source, and compact 175–200% drawers. Illustrated environment detail and richer timetable/service animation remain later bounded polish; final Phase 1.5 closure still requires owner acceptance of the four comparison boards.
