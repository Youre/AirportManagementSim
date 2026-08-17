<!-- ai-project-init:begin managed -->
# Current State

Project: AirportManagementSim
Material profile: documents
Initialization mode: augment-existing

Active priorities:
- Use the recovered root UI shell as the interaction baseline. Decorative
  activity and context canvases are self-hit-test-invisible, so only their
  actual controls consume pointer input. A successful load always returns to
  the airport overview. Terminal tools are contextual overlays on that same
  airport canvas; they must not replace the root shell, hide the rest of the
  airport, or move/reset the player-owned management camera. Regional, Advanced,
  and Major remain explicit destinations, retain a visible Back action, and
  never open one another automatically.
- Compose presentation revisions with a non-serialized restore epoch. This
  invalidates UI and Paper2D caches after every successful save restore even
  when the loaded domain revisions equal the current revisions. Do not remove
  this epoch or compare raw restored revisions directly in presentation code.
- Keep the schema-10 terminal as the only authoritative terminal
  presentation. It is permanently roofless: floors, walls, doors, furniture,
  construction, workers, and occupants remain at one footprint, anchor, and
  scale in every gameplay mode. Terminal tools change interaction and chrome,
  never world visibility or camera state. Mixed-
  perspective facility artwork is fixed north-up and must not be rotated;
  replace it with strict-nadir procedural geometry or reviewed directional
  variants rather than compensating with transforms.
- Preserve the doubled terminal presentation contract. Saved terminal cells
  remain the one-meter logical edit unit, but the airport renderer and pointer
  deprojection share a sixteen-meter world-presentation module. The furnished
  starter footprint is approximately 288x192 meters, the grown regional
  footprint is 480x288 meters, static furnishings fill 84 percent of their
  authored sixteen-meter-cell footprint, and people, bags, workers, and
  vehicles use a separate twelve-times readability scale. The fixed landside-
  campus anchor places the full footprint between the apron and access road
  without moving the camera.
- Treat the mature exterior overview as the default presentation state after
  every direct load, Back action, and destination close. The retired fixed-
  coordinate Phase 3 terminal proof surfaces, routes, and props must remain
  hidden there. Large scale-sensitive mature facilities now use deterministic
  procedural footprints, borders, seams, entrances, lanes, and markings;
  retain sprites only for small identity-bearing aircraft, people, vehicles,
  fixtures, and reviewed landscaping.
- Apply the terminal object's shared clockwise 90-degree art-basis correction
  only in the Paper2D presenter. Saved and player-authored `QuarterTurns`,
  footprints, routes, walls, doors, and source sprites remain authoritative.
  A non-empty schema-10 terminal snapshot must render after direct restore
  without depending on a preceding Phase 1 world-presentation pass.
- Continue VA-03 visual remediation after the usability recovery. The spatial
  terminal is now coherent and operable, but it remains materially less dense
  and expressive than the concept: people, furniture, route overlays,
  curbside context, gate/apron activity, and finer surface hierarchy remain
  visual debt. Preserve the working navigation/input shell while improving
  those bounded systems.
- Use the shared procedural infrastructure renderer for runways, taxiways,
  service roads, aprons, gate pads, construction progress, and placement
  previews. Geometry, markings, borders, panel seams, and safety envelopes are
  dimension-driven and use the same world-space layer contract in build and
  normal play. Connection-node sprites remain deliberate interaction cues;
  do not restore scale-stretched movement-surface sprites as authoritative
  geometry.
- Use the packaged `VisualBaseline` slot as the canonical repeatable visual and
  interaction review state. Install or verify it with
  `scripts/ui/Install-VisualBaselineSave.ps1`; use `-Refresh` only after a
  material fixture or presentation change. Load it from the canonical package
  and return from Regional directly to the mature exterior airport.
- Continue VA-01 remediation one bounded visual system at a time. The compact
  one-line header, header-adjacent time controls, map-dominant shell, inward-
  expanding right activity rail, hidden locked destinations, and Paper2D-only
  world path are now the baseline. The next system is the root selected-
  aircraft inspector, followed by denser airport-world composition and concise
  category-specific activity drawers with badges.
- Preserve the recovered VA-02 build-planning language in the authoritative
  Paper2D world: subdued parcel grid and boundary, translucent cyan/coral plan
  fills, diagonal proposal patterns, compact endpoints, and distinct snap,
  connection, crossing, and blocked nodes. All new airport surfaces must use
  the shared terrain/road/runway/taxiway/gate/structure/proposal/marker layer
  contract so roads and movement surfaces remain below gates and terminal
  artwork while interaction markers remain legible above them.
- Keep one authoritative Paper2D airport-world renderer in every interaction
  mode. Build mode is UMG tool chrome over the live `AAMSimWorldPresenter`;
  committed infrastructure, the schema-10 terminal, gates, and transient
  construction previews all use shared parcel/world geometry. Do not restore
  UMG facility substitutes or widget-local map transforms.
- Use the schema-10 spatial terminal as the authoritative starter-terminal
  presentation from airport creation onward. The normal overview and every
  tool mode show its furnished roofless interior at the fixed landside-campus
  anchor. Opening Terminal exposes contextual tools only. Never restore the legacy
  regional-terminal sprite after build-mode or visibility transitions. The
  apron-stand art remains authoritative for the two starter gates.
- Use the corrected airport-naming card as the initial-creation baseline: a
  centered, bounded lower-map surface with aligned parcel context, a wider
  airport-name field, and one primary action. Do not restore the former
  full-width banner or equal-width form columns.
- Use monotonic starter construction stages as the Phase 1 baseline. Inspection
  no longer regresses to Surface Work on alternating updates; the compact card
  should show one stable inspection state until Ready to Open.
- Use the corrected post-construction control loop as the Phase 1 progression
  baseline. Build reopens committed-airfield status, Schedule owns the paused
  first-visit timeline and deliberate 1x/arrival/4x pacing choices, and
  Overlays exposes real Airfield/Connections/Activity modes. First-visit
  completion now points to the player-facing Start Living Airport action.
- Run owner gameplay tests only against the canonical current package at
  `D:\AMSim-Current\Windows\AMSim.exe`. Refresh that same package after relevant
  gameplay or presentation changes; do not direct the owner to repository
  `Binaries`, staged-build internals, or dated historical packages. Verify the
  three rail paths, automatic return to 1x at inbound, and Living Airport
  continuation there.
- Use the verified owner-interaction correction as the current Phase 1 input
  baseline: ordinary hover is silent, taxiway bodies start connected branches,
  the construction truck uses its corrected art axis, and Staff exposes the
  four-worker automatic-dispatch summary.
- Treat the autonomous first-GA-visit journey as the Phase 1 operational
  baseline: scheduled aircraft stay off-map, movement follows the committed
  runway/taxi network, the 1x journey lasts about one minute, and speech calls
  serialize without interruption. The next owner replay should judge pacing
  and local-voice cadence with sound enabled.
- Use the compact icon-only map rail and concise corner activity card as the
  baseline for future HUD tools. New rail actions must reuse the expanding tool
  button and supply icon, hover/focus label, tooltip, and disabled explanation.
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
- Load `VisualBaseline` in `D:\AMSim-Current\Windows\AMSim.exe`, confirm the
  roofless terminal interior is already present at doubled campus scale on the
  airport canvas, its seats/desks/amenities and moving identities are readable
  at overview zoom, and the footprint clears the landside access road, then
  open and close Terminal tools without any world or camera transition. Pan and
  zoom from the airport overview into the interior using the same management
  camera. Then run the owner's build-mode visual acceptance journey, including
  runway placement, connection-node discovery, a runway-to-gate taxi link, and
  a service road crossing a gate footprint.

Last verification:
- 2026-08-16 EDT: terminal contents now match the doubled campus proportions.
  Static furniture and exact 2/4/6-seat compositions use the full sixteen-meter
  presentation cell with the existing aspect-preserving 84-percent footprint
  fill. Passengers, visitors, workers, bags, staff, and service vehicles use a
  separate twelve-times identity scale, making them readable without changing
  their anchor cells or gameplay occupancy. The same-state packaged
  before/after proves the prior dots are now legible furniture and activity;
  room boundaries, routes, camera, renderer, schema, and simulation remain
  unchanged. Focused automation and all 82 AMSim tests pass. Clean Development
  and Shipping packages each cook 750 packages; the Phase 7 release audit is
  clean. The canonical windowed launch passes with all 106 prior Saved files
  restored byte-for-byte; inner executable SHA-256 is
  E9F70D254097C9CA91985A81721DA606614C45605534BA5E3EA309E713B9C4B2. C
  retains 125.05 GiB and D retains 669.58 GiB free; project caches measure
  8.61 GiB, shared Unreal caches measure 0.81 GiB, and no cache was cleared.
- 2026-08-16 EDT: the owner-rejected eight-meter terminal correction is
  superseded by the doubled airport-campus contract. One shared presentation
  geometry contract maps the 18x12 starter layout to approximately 288x192
  meters and the 30x18 regional layout to 480x288 meters; furniture and moving
  entities retain a bounded six-times readability scale. A fixed
  presentation-only campus anchor places the grown footprint between the apron
  and landside access road while rendering and pointer deprojection keep the
  same center. The terminal now occupies roughly one quarter of the packaged
  1920x1080 world width and is comparable in prominence to VA-01. The
  single world renderer, permanently roofless interior, and player-owned
  camera remain unchanged. Focused automation and all 82 AMSim tests pass.
  Clean Development and Shipping packages each cook 750 packages; the Phase 7
  release audit passes with zero runtime string loads, forbidden dependencies,
  required 3D candidates, or line violations. The retained doubled-scale VA-01
  comparison is under
  `docs/planning/50-production/terminal-growth-visual-validation`. The
  canonical package passed a real windowed launch with all 106 prior Saved
  files restored byte-for-byte; inner executable SHA-256 is
  26D4725C40C3051D168C53702F3CA76B31557796366E20C7DF63FBDE0755135B. C
  retains 125.06 GiB and D retains 671.31 GiB free; project Intermediate is
  8.32 GiB, shared Unreal caches are 0.44 GiB, and no cache was cleared.
- 2026-08-16 EDT: the terminal is now permanently roofless on the one
  authoritative airport canvas. Opening and closing Terminal tools changes
  only the left/right contextual panels; the camera location, player zoom,
  mature airport, and schema-10 terminal world geometry do not change. One
  adaptive player-controlled zoom curve spans the full airport overview down
  to close interior inspection without a facility mode. The fixed-coordinate
  Phase 3 proof renderer remains retired. Focused automation and all 82 AMSim
  tests pass; clean Development and Shipping packages cook 750 packages; the
  Phase 7 release audit is clean. Packaged same-frame captures are retained in
  `AMSim/Saved/SingleCanvasTerminalAudit`. The canonical package was refreshed
  with all 103 saved files preserved byte-for-byte; its inner executable
  SHA-256 is
  EC5D8103A1391993F14180DF020D873A84F9C7622680659E99D2C7D3C57824E4.
  The terminal remains physically small at full-airport zoom; that is scale
  and composition debt rather than a second renderer. C retains 127.07 GiB
  free and no Unreal cache was cleared.
- 2026-08-16 EDT: Terminal is now an in-place contextual cutaway on the
  persistent airport canvas rather than a replacement destination. Exact-frame
  packaged captures retain the root shell, runway, apron, roads, aircraft,
  landscaping, mature facility geometry, and camera position while only the
  terminal roof/interior state and contextual tools change. Pan and zoom remain
  continuous through open and close, and the hidden Regional presenter no
  longer overwrites the terminal visibility state. All 82 automation tests and
  clean Development/Shipping packages pass. The Phase 7 release audit reports
  750 cooked packages and zero runtime string loads, forbidden dependencies,
  required 3D candidates, or line-limit violations. The canonical inner
  executable SHA-256 is
  271D4460E214962D0750C731219FAC23F89F0DC8552EDEC2B4F8AB48C920058C.
  VA-03 density, routes, occupants, curb/gate context, and richer materials
  remain visual debt. C retains 127.77 GiB and D retains 681.67 GiB free; no
  Unreal cache was cleared.
- 2026-08-16 EDT: direct save restore and destination close now explicitly
  enter the mature exterior overview, so the retired Phase 3 terminal proof
  renderer cannot leak into the airport map. Eight scale-sensitive mature
  facility families now use 24 strict-nadir procedural base, surface, and
  detail meshes with explicit layer separation; only one bounded apron-fixture
  sprite remains from the old mature-site pool. A packaged VisualBaseline ->
  Terminal -> Back replay confirms a coherent exterior, the authoritative
  schema-10 cutaway, and stable return. All 82 automation tests, clean
  Development and Shipping packages, both launch smokes, and the Phase 7
  release audit pass. The canonical package was refreshed with all 92 saved
  files restored byte-for-byte; its inner executable SHA-256 is
  C9681F2FD307C5ABC48EBB63049CF17F36DE1ED07B117A9F616548AD7B1E5D5A.
  VA-01 still records facility texture, landscape/activity density, and the
  bottom contextual inspector as continuing polish debt. C retains 128.36 GiB
  free and no Unreal cache was cleared.
- 2026-08-16 EDT: airport movement surfaces now use a cooker-visible,
  vertex-colored procedural mesh renderer instead of sprite-scale geometry.
  Phase 1 previews, construction progress, completed runways/taxiways/roads,
  gates, and the mature `VisualBaseline` airport share the same dimensional
  builders and explicit presentation layers. A VA-01 correction added darker
  surface hierarchy, scalable shoulders, apron borders/panel seams, gate
  guidance, and safety envelopes while preserving connection markers. The
  packaged equivalent-state capture confirms continuous networks and props
  above road/gate surfaces. All 82 automation tests pass; clean Development
  and Shipping packages pass; the packaged smoke and Shipping launch check
  pass; and release scans report zero runtime string loads, forbidden
  editor/MCP/test dependencies, required 3D assets, or line-limit violations.
  The canonical inner Development executable SHA-256 is
  C627F796E93CDC35C64B6AF7688449ACB7771881E0E978C573108720364C2214.
  C retains 128.43 GiB free and no Unreal cache was cleared.
- 2026-08-16 EDT: the canonical Development package now contains a reusable
  `VisualBaseline` schema-10 save derived from the deterministic Phase 4
  journey. The packaged state loads and returns directly to the complete
  exterior airport. The first VA-01 shell correction condenses the header,
  groups time controls with status, replaces the permanent 320-unit diagnostic
  panel with an inward-expanding activity rail, hides locked destinations, and
  removes unused UMG world placeholders. The before/after comparison confirms
  materially more world area while honestly retaining the bottom selection
  inspector, world-density, and activity-detail gaps. All 81 automation entries
  pass (80 success plus one retained warning), the source release audit passes,
  and the canonical inner executable SHA-256 is
  03B27A68B92E19A24C3F4AC63805099C705336D554DA2318FBCD3BF63DE068EA.
  Cache usage remains healthy and no Unreal cache was cleared.
- 2026-08-16 EDT: the unified Paper2D build world now restores the missing
  VA-02 planning overlay and connection language. Proposal surfaces use
  source-size-correct translucent fills, outlines, and diagonal patterns;
  snap/connected/crossing/blocked markers are distinct and camera-scaled; and
  a shared height/sort contract keeps roads, runways, and taxiways below gates
  and terminal artwork. A live 1280x720 replay proved the connected network and
  a deliberate road/gate overlap, all 81 automation tests passed, the Phase 7
  release scan reported zero runtime string loads or forbidden dependencies,
  and a clean 748-package Development archive plus packaged launch smoke
  passed. The canonical package was refreshed with all 24 saved files restored
  byte-for-byte; no shared Unreal cache was cleared.
- 2026-08-16 EDT: build mode and normal play now share the authoritative
  Paper2D world. The duplicate UMG terminal/gate/runway/taxiway/road renderer
  and its widget-local coordinate model were removed; transient plans use a
  bounded Paper2D proxy pool and the same geometry helpers as committed
  infrastructure. All 81 automation tests pass, clean UE 5.8 Editor/Game and
  Development/Shipping builds pass, packaged Development smoke passes, and
  the Shipping launch remains responsive. A real 1280x720 replay confirmed
  terminal/gate continuity, direct runway/taxiway placement, network-ready
  validation, and no position jump on confirmation. Release scans report zero
  runtime string loads, forbidden dependencies/package matches, and line-limit
  violations. The canonical package was refreshed with all 22 saves restored
  byte-for-byte; no shared cache was cleared.
- 2026-08-08 EDT: the normal new-airport presentation now sends the Phase 3
  terminal-layout snapshot to the world before the Terminal screen is opened.
  Focused automation proves one generated roof proxy per built floor cell,
  authoritative starter-facility anchoring, and legacy-sprite suppression
  across build-mode transitions. The packaged integrated Phase 1 journey and
  terminal cutaway proof pass; visual inspection confirms the readable roof
  at the service-road endpoint and the furnished interior after opening
  Terminal. The canonical `D:\AMSim-Current\Windows\AMSim.exe` package was
  refreshed with matching binary/container hashes while preserving saves.
- 2026-08-08 EDT: the starter terminal/gate correction passes the UE 5.8
  Editor/Game builds, the orthographic-camera regression, all six focused
  Phase 1.5 presentation tests, equivalent-state 1920x1080 build-mode and live
  Paper2D captures, a clean 658-package Development cook/stage/archive, and an
  integrated packaged Phase 1 smoke. The canonical package at
  `D:\AMSim-Current\Windows\AMSim.exe` was refreshed in place; no new art was
  needed because the approved regional-terminal and apron-stand assets already
  existed but were not wired consistently.
- 2026-08-08 EDT: the compact left rail now preserves each icon's authored
  aspect ratio and gives translated hover/focus labels a non-clipping host plus
  a 156-unit minimum card. UE 5.8 Editor/Game builds, focused CompactHud
  automation, a real 1280x720 hover proof, and the clean canonical Development
  package pass. `D:\AMSim-Current\Windows\AMSim.exe` was refreshed in place.
- 2026-08-08 EDT: the airport-naming card alignment correction passes the UE
  5.8 Editor/Game builds, focused CompactHud automation, and the established
  1920x1080 rendered Phase 1 smoke. A separate real 1280x720 game capture now
  guards the window-size breakpoint that the owner exposed. Both captured
  initial states confirm the bounded lower-map card, wider name field, shared
  vertical alignment, and unobstructed central map.
- 2026-08-03 EDT: corrected the authoritative Building/Inspection oscillation.
  Focused construction automation, complete 76-result automation, Editor/Game
  builds, and Development BuildCookRun pass. The refreshed package is under
  `D:\AMSimPlayable-20260803-FirstVisit`.
- 2026-08-03 EDT: the Phase 1 post-construction dead end is technically
  corrected. Editor/Game builds, focused operations-hub and Paper2D overlay
  tests, complete 76-result automation, and Development BuildCookRun pass.
  The new package launches windowed; interactive computer-use validation
  stopped at the new-path Windows Firewall prompt without changing it.
- 2026-08-03 EDT: Editor and packaged Development builds pass, all 75
  automation tests pass, and a real Windows replay confirms the Staff panel
  starts closed, opens clear of the rail, toggles closed, and reports 4/0
  availability before purchase. The same replay places a runway, completes a
  runway-to-Gate-A taxiway, then starts a second segment from its center to
  Gate B without moving the first segment.
- 2026-08-02 EDT: the first GA visit now follows the actual runway and complete
  taxi network through approach, landing, rollout, taxi-in, a 40-game-minute
  automatic turnaround, taxi-out, takeoff, and outbound flight. Scheduled and
  completed aircraft remain off-map. Editor/Game builds, all 75 automation
  tests, and the integrated 1920x1080 journey/save-load/render smoke pass;
  speech FIFO coverage proves calls cannot interrupt each other. The next
  owner replay should judge subjective pacing and voice cadence with sound.
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
  introduced acknowledged contextual-help IDs; schema 9 added taxiway-segment
  graphs, and the current schema 10 adds the growable terminal spatial domain
  while migrating schemas 1-9.
  Six first-use cards, the five-tab Release Guide, a local accessibility
  profile, centralized captions-first radio, 15 phrase families, and 15
  release Primary Assets are integrated without adding simulation mechanics.
- The Phase 7 actual-product audit compares live Unreal captures to VA-01
  through VA-07 and records three material differences, intentional
  differences, and a bounded correction for every state. Regional, Advanced,
  and Major are explicit destinations; Terminal is an in-place airport-world
  cutaway so later eligibility no longer obscures concept-mapped surfaces.
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
- The construction-motion follow-up replaces the building-stage surface sweep
  with short staggered out-and-back work patrols. Two crews cover separate
  runway zones, one follows an active taxi segment, and one follows the optional
  road or gate-side taxi route. Patrols continue through inspection; every
  moving worker and the delivery truck faces its current direction, reversals
  are continuous, and pause preserves both position and facing. Focused and
  complete automation pass 1/1 and 73/73, editor/game builds pass, and the
  integrated 1920x1080 construction captures pass visual inspection.
- The 2026-08-03 owner-interaction correction keeps visual hover/focus but
  removes repetitive pointer-enter audio from every themed button. Taxiway
  endpoint handles remain editable while a body press starts a snapped new
  branch rather than translating an existing segment. The truck applies the
  source-art-specific 180-degree correction. Staff is now an enabled Phase 1
  rail action backed by an immutable-state presenter and a concise, dismissible
  four-worker availability/assignment card.
- The 2026-08-02 UI audio pass approves and integrates all 28 nonverbal MP3
  source masters in `SourceAssets/Audio/UI`. Checksums match the generation
  manifest; Unreal imports short stereo, non-looping UI sound waves under
  `/Game/Audio/UI` with non-destructive reviewed level trims.
- `UAMSimUISoundSubsystem` owns the typed cooker-visible catalog, per-cue
  cooldowns, and `-NoSound` behavior. The shared Riverbend button style gives
  current and future themed buttons press audio automatically while ordinary
  mouse-over remains intentionally silent; explicit
  result cues cover construction, offers, schedules, persistence, warnings,
  objectives, capability changes, incidents, and recovery.
- UI-audio verification passes 3/3 focused tests and the complete 72-test
  suite (71 clean plus the retained warning-only backup probe). Clean
  Development and Shipping packages contain all 28 sounds; the package audit,
  offline runtime smoke, dependency/runtime-load scan, and release attestation
  pass with zero missing cooked sounds. Subjective mix approval remains part
  of the next owner gameplay listen rather than an automated claim.
- PI-11 delivers the growable-terminal starter vertical slice. New airports
  receive a built, editable 18x12 GA terminal; initialized passenger fixtures
  receive a spatial 30x18 grown terminal. One-meter logical floor cells,
  wall/door edges, rotatable objects, inferred rooms, construction jobs,
  closures, transactions, and autonomous visitors persist in schema 10; their
  airport-world presentation uses the doubled-scale module documented above.
- Terminal editing uses full-world mouse deprojection, immediate Phase 1 ledger
  spending, no-mutation invalid/unaffordable previews, full queued/in-progress
  undo refunds, completed demolition with 25% salvage, and ordinary moving
  construction workers. The schema-10 interior is permanently roofless on the
  live Paper2D airport canvas. The root presentation supplies the spatial
  snapshot before Terminal tools are opened; the authoritative service-road
  anchor is shared and the legacy sprite stays suppressed.
- Opening Terminal retains the airport header, footer, side rails, full
  exterior world, terminal interior, and exact player-owned camera frame. Its
  contextual tools are layered over the airport canvas. Closing them changes
  no world proxy, pan position, or zoom and restores no alternate camera state.
- The owner approved both OpenAI visual targets. The retained deterministic
  kit contains 45 source textures and 45 PaperSprite companions under
  `/Game/TerminalGrowth`; rejected ComfyUI surface candidates remain outside
  runtime content. Comparison boards record continuing density, route-overlay,
  and surrounding-world polish debt without expanding this rollout to upper
  floors or the passenger construction catalog.
- Terminal furnishings now use strict-nadir, footprint-safe presentation. A
  project-owned modular seat composes exact two-, four-, and six-seat groups,
  back-to-back rows face opposite directions, and all other placed furnishings
  use uniform aspect-preserving footprint fit instead of independent-axis
  stretching. The Phase 3 layout, saves, routing, economy, and construction
  contracts remain unchanged.
- Final PI-11 verification has zero failures across 81 discovered AMSim
  automation entries, passes the Phase 4 rendered fixture and the starter,
  grown, and component-gallery states at all five UI scales, and passes clean
  Development/Shipping packaging plus packaged
  terminal rendering, save/load, and offline checks. The technical release
  audit has zero runtime string loads, forbidden dependencies, required 3D
  assets, or line-limit violations; only the separate Phase 7 owner-acceptance
  field remains pending.
- `ai-init doctor . --json` runs from `PATH`; its retained warnings are the
  documented baseline managed-template, lexical-review, legacy-ledger, and
  project schema/version warnings.
