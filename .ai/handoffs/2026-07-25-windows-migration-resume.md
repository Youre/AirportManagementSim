# Windows Migration and Codex Resume Handoff

**Status:** Completed on replacement workstation
**Created:** 2026-07-25  
**Owner:** Current Codex session on the original Windows workstation  
**Next owner:** Future Codex session on the replacement Windows workstation  
**Repository:** `https://github.com/Youre/AirportManagementSim.git`  
**Branch:** `codex/planning-docs-concept-art`  
**Objective:** Recreate a clean development environment and resume at Phase 0 without repeating concept or specification work. Completed; retained as historical evidence.

## Critical first instruction

Check out `codex/planning-docs-concept-art`. The approved planning baseline is not on `main` as of this handoff.

```powershell
git clone https://github.com/Youre/AirportManagementSim.git
Set-Location AirportManagementSim
git switch --track origin/codex/planning-docs-concept-art
```

If the repository already exists:

```powershell
git fetch origin
git switch codex/planning-docs-concept-art
git pull --ff-only
```

Do not begin implementation until the branch, handoff, and planning files are present.

## Required reading order

1. `AGENTS.md`
2. This handoff
3. `.ai/context/current-state.md`
4. `.ai/constitution/*`
5. `docs/planning/README.md`
6. `ideas.txt`
7. `docs/planning/00-foundation/specification-index.md`
8. `docs/planning/50-production/phased-implementation-roadmap.md`
9. `docs/planning/50-production/verification-traceability.md`
10. `docs/planning/30-content-and-assets/concept-art/README.md`
11. Every owning specification listed for Phase 0 before changing the Unreal project

Treat `ideas.txt` as product intent and the approved specifications as normative implementation behavior. Treat concept art as subordinate visual evidence, never as numerical or technical authority.

## Historical project state at handoff

- Concept brainstorming is complete and organized in `ideas.txt`.
- The planning workspace contains 39 indexed specifications:
  - 4 player-experience specifications;
  - 20 gameplay-system specifications;
  - 4 content/asset specifications;
  - 9 technical specifications;
  - 2 production/integration specifications.
- The seven reviewed concept-art images cover overview/HUD, build mode, terminal flow, timetable, turnaround, weather/incidents, and specialization progression.
- The aircraft research inventory contains 46 source sprite records. No source aircraft asset has been copied or approved for import.
- Gameplay implementation had not started at the time of migration.
- `AMSim/` is the current and correct Unreal project folder. Earlier notes may mention `APSim`; do not recreate or rename back to that obsolete name.
- `AMSim` is a minimal Unreal Engine 5.8 C++ scaffold with `Config`, `Source`, `.uproject`, and `.vsconfig`.
- Generated Unreal directories, solutions, caches, logs, crash dumps, and binaries are intentionally excluded by `.gitignore`.

## Approved architecture baseline

- Unreal Engine 5.8.
- Strict 2D top-down presentation using an orthographic camera and Paper 2D.
- Windows desktop, mouse and keyboard, offline single-player.
- Lightweight deterministic C++ simulation records with stable IDs.
- Actors, sprites, widgets, animation, and audio are presentation rather than authoritative state.
- Commands, immutable events, query snapshots, and versioned save snapshots form system boundaries.
- Asynchronous SaveGame persistence with migration, atomic replacement, and previous-known-good recovery.
- CommonUI/UMG for interface composition.
- Unreal MCP and related automation are localhost, editor-only, and excluded from Shipping.
- No gameplay backend, accounts, telemetry, cloud dependency, multiplayer, or 3D gameplay assets.

## Source-machine hardware blocker

Do not use the original workstation for Unreal shader compilation or build validation.

The original workstation has an Intel i9-14900K with:

- MSI MPG Z790 EDGE WIFI BIOS `H.B3` dated 2024-03-12;
- CPU microcode `0x123`;
- 52 WHEA hardware events in seven days;
- repeated processor-core internal parity and TLB errors;
- one fatal WHEA hardware record and an unexpected reboot;
- nine UnrealEditor crashes across ShaderPreprocessor, AssetRegistry, and unknown addresses.

This was diagnosed as CPU/firmware instability, with shader preprocessing acting as the trigger. No project change should be judged using build results from that machine.

Do not migrate these directories from the old machine:

- `AMSim/Binaries/`
- `AMSim/DerivedDataCache/`
- `AMSim/Intermediate/`
- `AMSim/Saved/`
- `AMSim/.vs/`
- generated `.sln` or `.slnx` files
- global Unreal Derived Data Cache

Clone from GitHub and regenerate locally.

## New-machine setup

1. Confirm Windows is stable and has no recurring WHEA processor or memory events.
2. Install Epic Games Launcher and Unreal Engine 5.8.
3. Install the Visual Studio/C++ workloads described by `AMSim/.vsconfig`.
4. Associate `AMSim/AMSim.uproject` with the installed Unreal 5.8 build. The current `EngineAssociation` GUID may be machine-local; use Unreal Version Selector if the editor is not found.
5. Regenerate IDE project files locally.
6. Do not copy or commit generated folders.
7. Run the smallest clean compile and package/bootstrap checks defined by Phase 0.

The current `AMSim/Config/DefaultEngine.ini` still contains blank-template maximum-quality 3D rendering settings such as DX12/SM6, ray tracing, Lumen-style GI/reflections, mesh distance fields, skin-cache shaders, and Substrate. These are not the approved 2D baseline. The first Unreal implementation task should replace them deliberately under TS-06 and Phase 0, with a documented before/after configuration review. Do not interpret them as product requirements.

## Verification already completed

- All 39 specification index links resolve.
- Planning documents are valid UTF-8.
- No local planning-document links are broken.
- Seven concept PNGs decode at 1672 x 941.
- Every concept image is referenced by multiple planning documents.
- Git ignore assertions proved that Unreal source/config/content remain trackable while generated artifacts are ignored.
- The planning commit `dd4592d8915be8a420e9723cc50a5316ca0803b4` was verified against the remote branch before this migration handoff.
- `ai-project-init doctor` currently reports one non-blocking warning: `.ai/context/verification-log.yaml` has stale generated managed content. The installed CLI does not expose the suggested `update-managed-block` repair command; no structural repository error was reported.

After cloning, rerun:

```powershell
$env:PYTHONPATH = 'C:\path\to\ai-project-init\src'
python -m ai_project_init.cli doctor .
```

If the custom skill is not installed on the new machine, the repository remains usable; report the missing external skill and continue with the checked-in `.ai` instructions.

## Changed files in the migration commit

The migration commit should include only portable project sources:

- `.gitignore`
- `.ai/**`
- `.codex/agents/**`
- `AGENTS.md`
- `project-os.yaml`
- `project-os.lock.json`
- `AMSim/.vsconfig`
- `AMSim/AMSim.uproject`
- `AMSim/Config/**`
- `AMSim/Source/**`

Planning documents, research screenshots, concept art, and `ideas.txt` were already committed in `dd4592d`.

## Historical blockers and open risks

- The branch is not merged into `main`; the new session must explicitly check it out.
- Real-aircraft names/data and source sprite rights still require content review.
- On the replacement workstation, no aircraft asset may be copied from `C:\Users\dave\Documents\Joes_Game\dist\assets` until CT-01's import manifest and rights review are approved. The deployment currently contains 45 matching PNGs; the inventoried Airbus A330-300 SVG is absent.
- TextToSpeech and Unreal MCP are experimental and must remain isolated behind documented fallbacks/editor-only boundaries.
- Reference-PC performance hardware has not been selected.
- The first successful Unreal launch and clean build were not yet verified at the time of handoff; they are now complete.

## Next concrete action

On the replacement workstation, verify the checkout and environment, then open Phase 0 from `docs/planning/50-production/phased-implementation-roadmap.md`. Begin with the project baseline/configuration audit and 2D renderer correction before gameplay systems or asset import.

Record all checks in `.ai/context/verification-log.yaml`, update `.ai/context/current-state.md`, and preserve this handoff as historical evidence after the migration succeeds.

## Replacement-workstation completion

Completed 2026-07-25:

- checked out and verified `codex/planning-docs-concept-art`;
- installed and associated Unreal Engine 5.8.0;
- regenerated ignored local project state;
- compiled `AMSimEditor Win64 Development`;
- completed a clean headless Unreal startup and shutdown;
- opened Phase 0 with the initial 2D/offline baseline audit;
- relocated the read-only aircraft source to `C:\Users\dave\Documents\Joes_Game\dist\assets`.

This file is now historical migration evidence. Current work is tracked in `.ai/context/current-state.md`.

## Phase 0 completion

Completed after migration:

- all 14 `AMSim.Phase0` automation tests passed;
- deterministic replay and save/load continuation passed;
- the orthographic CommonUI proof exceeded 60 FPS at 1920 x 1080;
- Development and Shipping packages passed boundary scans;
- the elevated inbound/outbound network-denied journey passed, with zero Shipping TCP sockets;
- the CT-01 zero-row manifest template passed and no aircraft source asset was copied.

See `.ai/reviews/2026-07-25-phase-0-verification.md` and `.ai/reviews/2026-07-25-phase-0-adversarial-review.md`. The next production step is Phase 1 grass-airfield discovery and architecture review, not more migration work.
