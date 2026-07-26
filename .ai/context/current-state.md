<!-- ai-project-init:begin managed -->
# Current State

Project: AirportManagementSim
Material profile: documents
Initialization mode: augment-existing

Active priorities:
- Begin Phase 1 grass-airfield vertical-slice discovery and architecture.

Current architectural tensions:
- Shipping must exclude editor and online-support dependencies.
- The Phase 0 high-end measurement host is not the release reference PC; select a representative reference tier before Phase 1 closes.

Next verification command:
- `.\scripts\phase0\Invoke-Phase0Pipeline.ps1 -PackageRoot 'D:\AMSimPhase0Final-20260726' -SkipFirewall`

Last verification:
- 2026-07-26 UTC: all 14 Phase 0 automation tests, Development/Shipping package pipeline, rendered 60 FPS proof, save/load continuation, Shipping boundary scans, and elevated network-denied smoke test passed.
<!-- ai-project-init:end managed -->

## Project-specific state

Active priorities:

- Open Phase 1 from `docs/planning/50-production/phased-implementation-roadmap.md`.
- Define the smallest complete grass-airfield journey and its architecture-impact review before implementation.
- Preserve the verified deterministic simulation, async save, strict 2D, and offline Shipping boundaries.
- Use the mapped concept-art comparison gates and keep the CT-01 manifest empty until an import/rights plan is approved.

Current tensions:

- Real-aircraft naming, performance data, and livery rights require later review.
- The experimental Unreal TextToSpeech and MCP plugins require isolation and fallback; neither may become a shipped dependency.
- Reference-PC selection remains open; the Phase 0 CORSAIR/RTX 5090 host is measurement evidence, not a release target.
- Three transitive editor plugin descriptors remain as staged metadata, but Shipping contains no corresponding editor modules or binaries.
- The proof uses `/Engine/Maps/Entry`; Phase 1 should replace it with a project-owned grass-airfield map.

Verification note:

- Migration to the replacement Windows workstation is complete.
- Unreal Engine 5.8.0 is associated with `AMSim`; the editor target compiles and headless startup exits cleanly.
- Phase 0 is complete. Its verification and adversarial records are under `.ai/reviews/`.
- The baseline replaces blank-template 3D renderer defaults with a DX11/SM5 scalable 2D configuration and explicitly opts into Paper 2D, CommonUI, and Enhanced Input.
- Six module boundaries now separate bootstrap, simulation, gameplay/save, UI, editor tooling, and developer tests.
- The deterministic empty-airport foundation has a 250 ms clock, stable IDs, named random streams, command/event/query contracts, replay/checksum, spatial occupancy, versioned snapshots, async verified saves, backup fallback, and definition validation.
- Fourteen `AMSim.Phase0` automation tests pass.
- The final 1920 x 1080 proof recorded 1164.596 average FPS, 1.488 ms p99 frame time, save/load continuation, and checksum `4404817232840225737`.
- Development and Shipping packages pass; Shipping has zero forbidden file/manifest/receipt matches and zero observed TCP sockets.
- An elevated inbound/outbound network-denied journey passes, and its temporary firewall rules clean up.
- Repository inspection and the initial 46-file aircraft inventory were completed on 2026-07-25.
- The replacement source `C:\Users\dave\Documents\Joes_Game\dist\assets` contains 45 dimension-matching PNGs; the inventoried Airbus A330-300 SVG is absent.
- CT-01 has a validated 21-column, zero-row import-manifest template. No aircraft source image has been copied into `AMSim/Content`.
- Seven reviewed concept-art references now cover overview, build mode, terminal flow, timetable planning, turnaround, weather/incidents, and specialization progression.
- Phase 1 gameplay implementation has not started.
- Until `ai-init` is installed on `PATH`, run the CLI module with the skill source on `PYTHONPATH`.
