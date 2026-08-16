# Unreal C++ / Blueprint / MCP Workflow

Treat this repository as a hybrid Unreal project. “C++ project” and “Blueprint project” are not mutually exclusive implementation modes.

## Responsibility boundaries

- Use C++ as the stable foundation for simulation, state, validation, persistence, performance-sensitive work, and durable UI behavior.
- Use Blueprint assets for presentation, composition, designer-tunable defaults, animation, and project-specific behavior built on exposed C++ APIs.
- For UI, prefer a native widget class that owns behavior and exposes deliberate `UPROPERTY`, `UFUNCTION`, delegate, and `BindWidget` boundaries, with a Widget Blueprint owning visual hierarchy and styling.
- Do not move visual layout into C++ merely because the repository has native source. Do not move core systems into Blueprint merely because MCP can mutate the asset.

## Airport-world rendering boundary

- `AAMSimWorldPresenter` and its focused Paper2D presenter modules are the only
  runtime owners of airport-world facilities, vehicles, people, overlays, and
  construction previews.
- UMG/CommonUI owns screen-space chrome: tools, cards, drawers, validation,
  instructions, and actions. It must not maintain a second airport renderer or
  substitute world geometry for terminals, gates, runways, taxiways, roads, or
  other placed facilities.
- Reuse `AMSimPhase1WorldGeometry` for parcel/world conversion in both
  directions. Do not add a widget-local parcel rectangle or a second
  world-to-screen coordinate model.
- Build mode is a tool/input state over the live Paper2D world. Committed and
  fixed context stays visible; uncommitted geometry uses bounded pooled preview
  proxies driven by immutable presentation state.
- Preview systems are presentation-only. They never write proxy positions back
  into simulation and must use the same geometry helpers as committed objects
  so confirmation changes state/treatment without changing position.

## Unreal MCP boundary

- Unreal MCP is an editor bridge, not a replacement for the C++ toolchain.
- Use source tools for C++ headers and implementations, module/build files, scripts, configs, compilation, and source-control work.
- Use Unreal MCP for editor-owned or reflected state such as assets, Widget Blueprints, levels, actors, materials, properties, editor inspection, PIE, logs, screenshots, and automation.
- Enable only the editor toolsets required for the task. Keep MCP and mutation toolsets editor-only unless a separately reviewed requirement explicitly calls for runtime MCP.
- Before mutation, inspect the available toolset and exact schemas. For UMG objects and slots, follow the bundled workflow: list properties, read the exact properties, then set them. Never guess reflected property names.
- After mutation, compile and save the asset, re-read it through MCP, exercise it in PIE, inspect logs, and retain only the intended project change.

## Runtime asset dependencies and cooking

- Do not rely on a runtime path string such as `LoadClass` or `LoadObject` alone to establish a cooker dependency.
- For a fixed required asset, prefer an explicit hard reference through a direct `UPROPERTY`, a Blueprint/default property, or constructor-time `ConstructorHelpers::FClassFinder` / `FObjectFinder`.
- Use `TSoftClassPtr` / `TSoftObjectPtr` when deferred loading is intentional, and register the asset through the Asset Manager, Primary Asset rules, or an explicit reviewed cook rule.
- Treat `DirectoriesToAlwaysCook` as a supplemental packaging policy, not a substitute for correct dependency modeling. Avoid broad directory rules that can hide missing references or over-cook content.
- Preserve an intentional runtime fallback when the product can safely degrade, but never accept fallback behavior as proof that the intended asset was cooked.

## Required verification for editor-authored runtime assets

Before considering an MCP-authored runtime asset integrated:

1. Build a clean Shipping package.
2. Inspect the actual package container format (`.utoc`/IoStore or `.pak`) and confirm the asset is present.
3. Exercise the affected path in the packaged Shipping build and verify the intended asset and behavior, not merely the fallback.
4. Re-run the forbidden editor/MCP/toolset dependency scan.

PIE proves editor/runtime integration but does not replace the packaged Shipping journey. Package presence proves cooking but does not prove interactive behavior.

## Canonical owner-test executable

- The single current owner-testing launcher is
  `D:\AMSim-Current\Windows\AMSim.exe`.
- Refresh that same clean Development package after relevant gameplay or
  presentation changes. Keep the path stable so the owner never has to choose
  between build artifacts.
- Never direct the owner to `AMSim/Binaries`, `Saved/StagedBuilds`, an inner
  packaged executable, Unreal Editor, or a dated verification package unless a
  specific diagnostic explicitly requires it.
- Dated packages are historical evidence only. The canonical package is the
  usability-testing surface; Shipping packages remain release-verification
  surfaces.
- Before handing off a refresh, verify the top-level launcher exists, record
  its timestamp/hash, and state whether packaged interaction was exercised or
  remains for owner validation.

## Runtime UI audio

- Retain generated or externally produced source masters outside `Content` and
  record provider, model, prompt/session, format, checksum, review decision, and
  runtime destination before import.
- Fixed required UI sounds use typed constructor-visible references or
  serialized UObject properties. Runtime string asset loading is prohibited.
- Put common button hover/press cues in the shared theme so future components
  inherit the approved interaction language. Use explicit semantic cues only
  after an authoritative action result or state transition.
- Rate-limit dense feedback such as hover, focus, magnetic snap, notifications,
  and alerts. Audio reinforces visible/captioned state and is never the only
  way to understand an outcome.
- Validate duration, channel count, looping, sound group, reviewed level trim,
  cooker presence, `-NoSound`, and a packaged interaction journey.

## Primary Epic guidance

- [Coding in UE: Blueprint vs. C++](https://dev.epicgames.com/documentation/en-us/unreal-engine/coding-in-unreal-engine-blueprint-vs-cplusplus)
- [Unreal MCP](https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor)
- [Referencing Assets](https://dev.epicgames.com/documentation/unreal-engine/referencing-assets-in-unreal-engine)
- [Exposing C++ to Blueprints](https://dev.epicgames.com/documentation/unreal-engine/exposing-cplusplus-to-blueprints-visual-scripting-in-unreal-engine)

Unreal MCP and its toolsets are experimental in Unreal Engine 5.8. Re-check the installed engine source and current Epic documentation when changing engine versions or relying on new toolsets.

## Cache and disk-space hygiene

- Run `scripts/unreal/Get-UnrealCacheUsage.ps1` before and after a clean
  `BuildCookRun`, and whenever system-drive free space approaches 20 GiB.
- Preserve at least 15 GiB free before starting a clean package. Stop new
  build/package work below 8 GiB until space is recovered.
- Build pipelines may report cache pressure, but must never delete caches
  automatically.
- Clean project-local reproducible output first: staged builds, cooked output,
  and then `AMSim/Intermediate/Build`. Retain required verification reports,
  screenshots, save fixtures, logs, source assets, and cooked-package evidence.
- Shared Derived Data, Zen, and UBA caches accelerate every Unreal project.
  Clear them only when project-local cleanup is insufficient and the owner has
  explicitly approved the broader cleanup.
- Before deletion, resolve every exact target, confirm no Unreal build/editor
  process is using it, and verify that it is a known cache directory. Never
  delete an Unreal, repository, workspace, `Content`, `Source`, `SourceAssets`,
  or `Saved` root recursively.
- After cleanup, record the exact cache categories removed, approximate space
  recovered, remaining free space, and the expected cold-build cost in the
  verification log or active handoff.
