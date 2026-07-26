<!-- ai-project-init:begin managed -->
# Current State

Project: AirportManagementSim
Material profile: documents
Initialization mode: augment-existing

Active priorities:
- Preserve the completed Phase 4 evidence and open Phase 5 only after an
  explicit planning session.

Current architectural tensions:
- Shipping must exclude editor and online-support dependencies.
- CommonUI and viewport capture must preserve a visible, interactive Windows mouse cursor.
- Automated screenshot checks prove capture integrity and layout bounds, not visual similarity or polish.

Next verification command:
- Before opening Phase 5, run `scripts/phase4/Invoke-Phase4Pipeline.ps1
  -SkipFirewall` if the Phase 4 foundation changes.

Last verification:
- 2026-07-26 UTC: Phase 4 builds and audits; all 46 automation records
  complete with zero failed/not-run/in-process results; VA-04/VA-06 and the
  100-200% matrix pass; clean Development and Shipping packages pass integrated
  S06/S10, schema-5 save/load, launch, socket, dependency, 2D, runtime-load,
  and direct 34-asset IoStore inventory gates.
<!-- ai-project-init:end managed -->

## Project-specific state

Active priorities:

- Keep Phase 4 stable and plan Phase 5 only when the owner opens it.
- Preserve a visible mouse cursor across gameplay world, HUD, panels, and click capture.
- Use OpenAI Image Gen for concept/UI exploration and local ComfyUI for reviewed simple textures, variants, edits, and cutouts.

Current tensions:

- The approved reference capability tier has no physical result yet, but its four-hour soak is deferred to Phase 7 rather than blocking continued development.
- The fictional Riverbend Trainer content lock is approved for this slice; any later real-aircraft naming, data, art, or livery still requires a separate CT-01/CT-02 review.
- Phase 4 is complete. Phase 5 remains unopened.

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
- Until `ai-init` is installed on `PATH`, run the CLI module with the skill source on `PYTHONPATH`.
