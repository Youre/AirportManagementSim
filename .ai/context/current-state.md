<!-- ai-project-init:begin managed -->
# Current State

Project: AirportManagementSim
Material profile: documents
Initialization mode: augment-existing

Active priorities:
- Use the verified slower starter-airfield construction balance and distinct
  marked movement-surface kit as the baseline for the next owner build-mode
  journey. Construction now takes 3 minutes 30 seconds at 1x, while 2x-8x and
  the useful-road reduction remain available.
- Resume the consolidated owner protocol now that the starter-network context
  correction has passed. The basic terminal and its two gates are fixed map
  context; the player draws runway and taxiway segments, while service roads
  are optional worker-speed infrastructure.
- Track Unreal cache usage before and after clean packages. Keep at least
  15 GiB free before packaging, stop builds below 8 GiB, and never clear shared
  DDC/Zen/UBA caches automatically.
- After that correction passes, resume the consolidated owner protocol:
  Phase 5 Advanced journeys, Phase 6 Major journeys, maximum-load observation,
  pacing journey, reference-tier soak, unassisted test, audio/captions, and
  final concept-art checkpoint.

Current architectural tensions:
- Shipping must exclude editor and online-support dependencies.
- CommonUI and viewport capture must preserve a visible, interactive Windows mouse cursor.
- Automated screenshot checks prove capture integrity and layout bounds, not visual similarity or polish.
- Maximum-scale deterministic automation does not replace the remaining
  time-based owner observation.

Next verification command:
- Run the next owner-selected acceptance journey from the consolidated Phase 7
  protocol against the schema-9 candidate.

Last verification:
- 2026-08-02 EDT: the runway-surface and construction-pacing correction passes
  all 73 automation tests, editor/game builds, integrated and packaged rendered
  smoke, a clean 658-package Development cook, direct IoStore asset inspection,
  offline socket observation, and runtime dependency/load scans. The final
  world uses separate marked runway/taxiway surfaces, preserves unmarked roads
  and earthwork, slows the 1x project to 3 minutes 30 seconds, and faces the
  09/27 overlays toward their corresponding approaches. Cache usage remained
  stable and no cache was cleared. Consolidated Phase 7 owner acceptance still
  remains.
<!-- ai-project-init:end managed -->

## Project-specific state

Active priorities:

- Keep Phase 1-6 behavior stable while owner acceptance is completed.
- Preserve a visible mouse cursor across gameplay world, HUD, panels, and click capture.
- Use OpenAI Image Gen for concept/UI exploration and local ComfyUI for reviewed simple textures, variants, edits, and cutouts.

Current tensions:

- The approved reference capability tier has no physical result yet, but its four-hour soak is deferred to Phase 7 rather than blocking continued development.
- The fictional Riverbend Trainer content lock is approved for this slice; any later real-aircraft naming, data, art, or livery still requires a separate CT-01/CT-02 review.
- Phase 4.5 is accepted with continuing visual polish debt. Phase 5
  implementation and technical verification are complete; owner accelerated
  acceptance remains.

Verification note:

- Migration to the replacement Windows workstation is complete.
- Unreal Engine 5.8.0 is associated with `AMSim`; the editor target compiles and headless startup exits cleanly.
- Phase 0 is complete. Its verification and adversarial records are under `.ai/reviews/`.
- The baseline replaces blank-template 3D renderer defaults with a DX11/SM5 scalable 2D configuration and explicitly opts into Paper 2D, CommonUI, and Enhanced Input.
- Six module boundaries now separate bootstrap, simulation, gameplay/save, UI, editor tooling, and developer tests.
- The deterministic empty-airport foundation has a 250 ms clock, stable IDs, named random streams, command/event/query contracts, replay/checksum, spatial occupancy, versioned snapshots, async verified saves, backup fallback, and definition validation.
- The current full-suite report contains 45 succeeded and one
  succeeded-with-warning record, with zero failed, not-run, or in-process
  tests.
- The final 1920 x 1080 proof recorded 1164.596 average FPS, 1.488 ms p99 frame time, save/load continuation, and checksum `4404817232840225737`.
- Development and Shipping packages pass; Shipping has zero forbidden file/manifest/receipt matches and zero observed TCP sockets.
- Phase 0 and Phase 1 elevated inbound/outbound network-denied journeys pass, and their temporary firewall rules clean up.
- Unreal 5.8 MCP now loads the editor and UMG toolsets editor-only. Its first retained mutation created `/Game/UI/WBP_PrimaryActionButton`, and the Phase 1 `Create airport` action uses that rounded cyan-outline template with a runtime fallback.
- The button class has a constructor-time hard reference and `/Game/UI` is also explicitly cooked. The directory rule is supplemental and not yet proven necessary because the first absence check inspected `.pak` rather than the active IoStore container. Both package pipelines reject `EditorToolset` and `UMGToolSet` alongside the existing editor/MCP/test dependency denylist.
- `AAMSimPlayerController` and the CommonUI root preserve combined game/UI input, click-only capture, a visible cursor, and no viewport mouse lock. Focused automation, rendered smoke, and a human click-through pass.
- `scripts/ui/New-VisualComparison.ps1` produces equivalent-state side-by-side boards and mismatch worksheets. The first VA-01 board exposed a text-panel-dominant proof shell; the corrective `Phase1.5-Comparisons-ComponentLanguage-Candidate` set records the rounded, reduced-copy replacement.
- The local ComfyUI server preflight passes at `http://192.168.5.12:8188`. Project guidance reserves OpenAI Image Gen for concept/UI work and permits local ComfyUI for reviewed simple textures, variants, edits, and cutouts.
- Repository inspection and the initial 46-file aircraft inventory were completed on 2026-07-25.
- The replacement source `C:\Users\dave\Documents\Joes_Game\dist\assets` contains 45 dimension-matching PNGs; the inventoried Airbus A330-300 SVG is absent.
- CT-01 retains a validated 21-column import-manifest template. The one owner-approved Cessna source is preserved under `SourceAssets/Phase1.5`, while cooked content contains only reviewed fictional-livery derived textures and sprites.
- Seven reviewed concept-art references now cover overview, build mode, terminal flow, timetable planning, turnaround, weather/incidents, and specialization progression.
- Phase 1 implementation is verified on the development host. It includes schema 2/migration, project-owned map, 20 Primary Assets, construction/cancel/refund/open/close, offer pin/decline/accept, exact timetable/stand buffers, persistent airframe operation/services/reward/recovery, query-backed UI, local radio, save/load, and clean packages.
- The replacement packaged S01 recorded 1,227.469 average FPS, 1.752 ms p99 frame time, 0.000 ms median/0.001 ms p99 1x simulation work, 7.524 ms save write, 501 MiB maximum resident memory, zero 8x backlog, checksum `6583174326702355518`, 2,200 Credits, 5 Airport Points, and five phrase intents.
- The corrective 100%, 125%, 150%, 175%, and 200% rendered UI matrix passes; medium layouts retain every action without clipping, and 175–200% uses compact mutually exclusive objective/operations drawers with a horizontally scrollable control strip.
- Shipping has zero forbidden file/text/receipt matches, zero observed TCP sockets, and zero required 3D gameplay candidates. Development has zero unexpected socket observations beyond the local trace listener.
- Phase 1 elevated S15 passed against the replacement package hashes: the Development journey and save/load completed at 1.819 ms p99 with only the expected local trace listener, Shipping observed zero TCP sockets during its five-second clean launch, and all eight temporary firewall rules were removed.
- The Phase 1.5 content review records the approved Cessna master path/checksum, cleanup, 32 pixels-per-meter definition, fictional Riverbend yellow livery, and 16 reviewed heading variants. No other external aircraft is imported.
- The exact four-binary Phase 1 package identity is tracked in `scripts/phase1/Phase1AcceptanceManifest.json`; the pipeline, offline result, both external recorders, content review, and final aggregate independently enforce the relevant source/package hashes.
- The portable reference-tier collector passes its five-second harness check, validates exact package identity, applies the documented profile, and deliberately reports formal acceptance false on the unreviewed RTX 5090 host.
- The privacy-safe tester recorder requires explicit first-time-session, consent where applicable, precondition, journey, comprehension, audio/caption/fallback, no-blocker, facilitator, and formal-evidence attestations. Non-certifying rehearsals cannot pass.
- The retained strict aggregate reports `referenceTierPassed` and `testerAcceptancePassed` false; those records are now Phase 7 release-hardening evidence rather than Phase 1 development blockers.
- Phase 1.5 corrective component language and technical verification were
  accepted on 2026-07-26.
- Phase 2 is complete. It adds schema 3, seven persistent airframes across four
  roles, recurring GA/school/charter operations, runway/reservations/towing,
  dependency-aware services, four vehicles, four staff teams, three tenants,
  land expansion, weather/deicing, a recoverable disabled-aircraft incident,
  ratings/achievements/recovery, query-backed UI, and pooled Paper2D world
  proxies.
- The three specialization fixtures each pass 14 deterministic operating days
  while solvent. The integrated packaged fixture reaches day 8 with 24
  completed flights, resolved incident, operational expansion, and save/load
  continuation.
- The final Shipping IoStore contains 300 packages, including a direct
  name-for-name match for all 33 Phase 2 Primary Assets. It has zero forbidden
  editor/test/MCP/Python/RemoteControl/Toolset matches, zero TCP sockets, zero
  Phase 2 runtime string asset loads, and zero required 3D gameplay candidates.
- Phase 3 is complete. It adds schema 4, staged domestic-terminal construction,
  seven typed network connections, controlled security and an accessible lane,
  52 named passengers in stable parties, 34 owned bags, RB 304, four passenger
  staff teams, a fictional passenger tenant/operator, and explicit private-car,
  taxi, bus, parking, curb, and terminal reconciliation.
- S05 completes all 52 passengers and 34 bags with one exact reconciliation
  pass. Save/load continuation passes from construction, security, boarding,
  baggage make-up, reclaim, and landside boundaries. The 10,000-logical and
  2,000-visible presentation contracts pass.
- The VA-03 terminal screen deliberately carries the concept's navy frame,
  warm/cool/purple/amber/green route language, labeled cutaway, named Maya
  party, needs/route/confidence hierarchy, and bottom legend. The 100-150%
  layout uses full rails; 175-200% uses bounded scrolling drawers and concise
  world labels with no P0-P2 mismatch.
- The final Shipping IoStore directly matches all 34 Phase 3 Primary Assets.
  Development packaged S05 and save/load pass; Shipping observes zero TCP
  sockets and has zero forbidden editor/test/MCP/Python/RemoteControl/Toolset,
  runtime string-load, or required 3D matches.
- Phase 4 is complete. It adds schema 5, three fictional recurring operators,
  a 21-flight seven-day timetable, exact slots, contact/remote gates and bus
  boarding, regional/narrow-body roles, connections, 32 transfer bags,
  international immigration/customs, rental/rail access, approach limits,
  a warned serious incident, report/recovery, and tenant renewal.
- S06/S10 completes 21 flights, connects 18 passengers, rebooks 6 missed
  passengers, completes 32 transfer bags, processes 96 international
  passengers, reconciles 42 rental-car and 58 rail passengers, records
  `No injuries`, and accepts one renewal.
- VA-04 and VA-06 use the accepted navy rounded component language. The
  100-150% layout retains seven-day rails; 175-200% uses compact data-derived
  labels and bounded scrolling without clipped critical actions.
- The final Shipping IoStore directly matches all 34 Phase 4 Primary Assets.
  Packaged Development S06/S10 and schema-5 save/load pass; Shipping observes
  zero TCP sockets and has zero forbidden editor/test/MCP/Python/
  RemoteControl/Toolset, runtime string-load, required 3D, or external Phase 4
  aircraft matches.
- Phase 4.5 visual-content remediation is active under CT-05/PI-07. The first
  content pass adds a mature-airport overview, pre-commit construction
  proposal, close-operations turnaround camera and dependency timeline, a true
  day/time timetable, a six-path capability map, 20 generated operations
  sprites, and 24 generated terminal/marking sprites.
- The shared Phase 4.5 UI atlas adds 16 project-owned tool, activity, status,
  and specialization identities with recorded prompt, cleanup, checksums, and
  cooker-visible hard references. The overview now includes icon-plus-label
  tools, activity symbols, aircraft identity art, schedule/stand, border,
  transport, readiness, connection, and baggage evidence. The capability map
  now uses six equal-weight identities, named Established/Regional/Advanced/
  Major bands, and requirements/current-evidence/next-capability/reward
  inspector sections.
- The generated mature-site atlas adds 16 cooker-visible runway, taxiway,
  apron, terminal, GA, operations, fuel, parking, transit, access, landscape,
  gate, and fixture sprites. The regional overview composes them with
  simulation-derived aircraft, vehicles, staff, passengers, and bags; other
  screens explicitly disable mature-overview mode. VA-01 now contains the
  required semantic airport categories. Added taxi and landside connections,
  perimeter landscaping, ground vehicles, people, bags, and a query-derived
  selected-flight inspector make the required complete-airport and mixed-
  activity outcomes present. The selected aircraft now has an explicit world
  ring bound to the inspector. Aircraft and Regional Terminal tabs now switch
  real identity art, query-backed readiness/capacity/service content, and the
  world focus ring. Contextual facility actions open the timetable and
  capability surfaces. Entity-specific drawers and still-higher ambient
  density remain optional polish beyond CT-05 rather than completion gaps.
- Seven equivalent-state comparison boards are recorded under
  `docs/planning/50-production/visual-reviews/phase-4-5/`. They confirm that
  the accepted color/component language remains coherent. Every CT-05 row now
  has a visible implementation or an intentional future lock; none remains
  absent or text-only. Construction has a six-category palette, authoritative
  invalid geometry and validation consequences; timetable has operator and
  aircraft identity plus a bounded weather window; turnaround has selected
  aircraft identity and parallel service progress. The terminal now uses
  textured architectural zones, a complete prop kit, continuous departure,
  arrival, baggage, and landside paths, plus a color-independent dashed
  accessible route. Incident capture settles after area protection, showing
  authoritative dark rain, a red hatched affected-aircraft closure, continuous
  response connectors, directional markers, three responding vehicles, and
  continuing airport activity. The terminal now adds authored partition and
  secure-door evidence, a dedicated clear/active baggage-exception branch and
  station, and family portrait art. Incident presentation adds separate
  cooker-visible runway, taxiway, and apron wet-surface layers.
- Construction now retains stage-derived work tint, truck, worker, cones, and
  protected-zone proxies through delivery/build/inspection and clears them on
  completion or cancellation. Turnaround adds a patterned exclusion zone, two
  authored approach arrows, explicit automatic-dispatch policy, safe equipment
  placement, and a dependency timeline. The timetable's selected contract has
  identity, silhouette, route, frequency, capacity, and reward; the affected
  weather cell has pattern plus text. The specialization inspector now adds
  spatial/business implications and an explicit capability-based,
  non-exclusive combination rule.
- Every implementable CT-05 requirement is present: coverage records 82
  present, zero schematic, and one approved future-locked assessment.
  Authoritative proof now shows an affected-only live taxi-spur closure with
  the runway and Stand A1 open, and localized weather and locked-horizon risk
  use full patterned labels on the affected timetable cell. The five-scale
  component gallery passes. Direct Shipping IoStore inspection finds the
  production root, all 48 Phase 1 presentation assets, and all 136 Phase 4.5
  runtime assets while excluding the development gallery and 16 intentionally
  unused PaperSprite companions for UI textures. The exact clean package rerun
  passes full automation, packaged Phase 1 and Phase 4 smoke, forbidden
  dependency/runtime-load/3D scans, and offline socket checks. Packaged manual
  interaction proves the cursor stays visible through viewport clicks, hover,
  focus, zoom, pan, time controls, captions, selection, and view navigation.
  The owner accepted this as the continuing-development baseline on
  2026-07-27; aesthetic refinement remains tracked visual debt.
- Phase 5 implementation is technically complete under PI-08. Schema 6
  composes cargo, provider tenants,
  rating contributions, three objectives, path-neutral achievements, six
  non-exclusive capability paths, and eight shared-lifecycle special events
  into `FSimulation`.
- Cargo supports four classes, outbound/inbound/transfer directions,
  dedicated feeder/regional freighters, compatible belly freight, warehouse
  compatibility/capacity, ordinary quantity-based dispatch, traceable
  exceptions/recovery, and Phase 1 economy/AP authority.
- Six Advanced fixtures prove GA, Flight School, Charter, Cargo, Passenger,
  and Mixed independence. Provider/concession offers expose footprint,
  opening/rent/share terms, capability, expectations, pattern, satisfaction
  drivers, term, and grace/recovery.
- Four Figma validation frames are retained under
  `docs/planning/50-production/phase5-visual-validation`. `/Game/Phase5`
  contains 44 definitions plus two project-authored fictional freighter
  texture/sprite pairs. No `Joes_Game`, external aircraft, military aircraft,
  runtime string load, or required 3D asset entered Phase 5.
- Final Phase 5 verification reports 5/5 focused tests, 54 passed plus one
  retained warning-only full-suite result, clean Development and Shipping
  packages, all 48 Phase 5 assets in IoStore, zero forbidden Shipping binary
  matches, and zero observed Shipping TCP connections.
- These automated and agent-executed checks do not substitute for the six
  owner-operated 10-15 minute prepared-save journeys required by PI-08. That
  is the sole Phase 5 acceptance gate still open.
- Phase 6 implementation is technically complete under PI-09. Schema 7
  composes Major progression, earned/operational capability state, parallel
  runway configuration, high-capacity facilities, wide-body operations,
  serious incidents, repairs/recovery, and scale diagnostics into
  `FSimulation`.
- Six independent fixtures reach Major for GA, Flight School, Charter, Cargo,
  Passenger, and Mixed with unrelated signature facilities absent. Progression
  remains non-exclusive and uses the focused Phase 5 evaluator.
- The Riverbend Longreach 787-9 uses the owner-approved River & Sun fictional
  livery. A project-authored vector master and 16 reviewed headings are
  cooker-visible; only the approved Boeing 787-9 reference is retained outside
  cooked content.
- The complete Phase 1-6 automation suite reports 62 clean passes, one
  retained warning-only legacy backup probe, and zero failures or not-run
  tests. The exact 10,000-logical/2,000-visible/150-aircraft/500-vehicle
  deterministic scale contract passes without a backlog warning.
- Fresh Development and Shipping BuildCookRun passes produce 613 cooked
  packages. Direct Shipping IoStore inspection finds all 60 Phase 6 assets
  and the production root widget; dependency, direct runtime string-load,
  required-3D, and eight-second offline socket checks are clean.
- Five required Phase 6 equivalent-state boards plus a supplemental
  maximum-scale frame cover parallel runways, high-capacity terminal flow,
  wide-body turnaround, Major capability, incident continuity, and maximum
  scale. The River & Sun aircraft checkpoint is approved; final owner visual
  approval remains open.
- Phase 6 formal closeout still requires the six Phase 5 Advanced journeys,
  six Phase 6 Major journeys, final visual checkpoint, and bounded 15-minute
  maximum-load observation. Phase 7 retains the four-hour soak, formal
  unassisted test, and genuine pacing journey.
- Phase 7 implementation is release-candidate ready under PI-10. Schema 8
  introduced acknowledged contextual-help IDs; the current post-RC schema 9
  adds taxiway-segment graphs and migrates schemas 1-8.
  Six first-use cards, the five-tab Release Guide, a local accessibility
  profile, centralized captions-first radio, 15 phrase families, and 15
  release Primary Assets are integrated without adding simulation mechanics.
- The Phase 7 actual-product audit compares live Unreal captures to VA-01
  through VA-07 and records three material differences, intentional
  differences, and a bounded correction for every state. Terminal, Regional,
  Advanced, and Major are explicit destinations so later eligibility no
  longer obscures concept-mapped surfaces.
- Phase 7 focused automation passes 5/5. The final complete suite reports 67
  clean passes, one retained warning-only Phase 0 backup probe, and zero
  failures or not-run tests.
- Clean Development and Shipping packages contain 628 packages, including all
  15 Phase 7 definitions. The audio-enabled packaged S01 journey and save/load
  pass; Shipping remains open for eight seconds with zero TCP sockets.
  Dependency, runtime string-load, required-3D, unreviewed-music, secret,
  endpoint, and source-line audits are clean.
- Riverbend RC1 is correctly marked `release candidate ready for owner
  acceptance`, not Phase 7 complete. The consolidated owner journeys,
  15-minute maximum-load observation, 10-15 hour pacing journey, four-hour
  reference-tier soak, formal unassisted/audio/caption protocol, and final
  visual approval remain pending by owner decision.
- The 2026-08-02 direct-placement correction replaces the starter-plan nudge
  controls with a compact runway/taxiway/stand-service palette and full-map
  click/drag placement. Live simulation validation now controls an explicit
  `BUILD AIRFIELD` action and actionable failure card. The new Development
  candidate passed packaged replay.
- The follow-up empty-network overhaul removes every pre-placed airfield
  element, gives runway and taxiway independent circular start/end drawing,
  accepts and marks interior taxiway/runway crossings, derives reciprocal
  runway numbers from heading, and renders the exact accepted geometry in the
  Paper2D world. The final 628-package Shipping candidate passed automated
  verification and a real Windows replay; owner visual acceptance remains.
- The 2026-08-02 starter-network context correction restores only a fixed,
  non-passenger basic terminal shell and two gate anchors. Runways, taxiways,
  and roads remain player-authored. Taxiways are validated as one graph from
  the runway to either gate, including intersections, branches, and indirect
  connections; orphan segments fail. Roads have no mandatory endpoint and
  grant a deterministic 20% construction-travel benefit only when useful.
- The construction-feedback pass now dispatches four visible workers and an
  approaching delivery truck immediately after a starter plan is funded. The
  accepted runway, taxiway graph, and optional road begin as full-length brown
  graded beds, reveal their final surfaces proportionally during `Building`,
  and gain runway markings only at inspection. Travel and reveal derive from
  authoritative simulation time, freeze while paused, and never alter saved
  geometry or simulation outcomes. Focused automation passes 4/4, the complete
  suite passes 72 clean tests plus the retained warning-only recovery probe,
  and both the integrated and refreshed packaged 1920x1080 rendered journeys
  pass. The current owner-test package is
  `D:/AMSimConstructionProgressDev/Windows/AMSim.exe`.
- The 2026-08-02 UI audio pass approves and integrates all 28 nonverbal MP3
  source masters in `SourceAssets/Audio/UI`. Checksums match the generation
  manifest; Unreal imports short stereo, non-looping UI sound waves under
  `/Game/Audio/UI` with non-destructive reviewed level trims.
- `UAMSimUISoundSubsystem` owns the typed cooker-visible catalog, per-cue
  cooldowns, and `-NoSound` behavior. The shared Riverbend button style gives
  current and future themed buttons hover/press audio automatically; explicit
  result cues cover construction, offers, schedules, persistence, warnings,
  objectives, capability changes, incidents, and recovery.
- UI-audio verification passes 3/3 focused tests and the complete 72-test
  suite (71 clean plus the retained warning-only backup probe). Clean
  Development and Shipping packages contain all 28 sounds; the package audit,
  offline runtime smoke, dependency/runtime-load scan, and release attestation
  pass with zero missing cooked sounds. Subjective mix approval remains part
  of the next owner gameplay listen rather than an automated claim.
- `ai-init doctor . --json` runs from `PATH`; its retained warnings are the
  documented baseline managed-template, lexical-review, legacy-ledger, and
  project schema/version warnings.
