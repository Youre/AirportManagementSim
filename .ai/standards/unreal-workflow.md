# Unreal C++ / Blueprint / MCP Workflow

Treat this repository as a hybrid Unreal project. “C++ project” and “Blueprint project” are not mutually exclusive implementation modes.

## Responsibility boundaries

- Use C++ as the stable foundation for simulation, state, validation, persistence, performance-sensitive work, and durable UI behavior.
- Use Blueprint assets for presentation, composition, designer-tunable defaults, animation, and project-specific behavior built on exposed C++ APIs.
- For UI, prefer a native widget class that owns behavior and exposes deliberate `UPROPERTY`, `UFUNCTION`, delegate, and `BindWidget` boundaries, with a Widget Blueprint owning visual hierarchy and styling.
- Do not move visual layout into C++ merely because the repository has native source. Do not move core systems into Blueprint merely because MCP can mutate the asset.

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

## Primary Epic guidance

- [Coding in UE: Blueprint vs. C++](https://dev.epicgames.com/documentation/en-us/unreal-engine/coding-in-unreal-engine-blueprint-vs-cplusplus)
- [Unreal MCP](https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor)
- [Referencing Assets](https://dev.epicgames.com/documentation/unreal-engine/referencing-assets-in-unreal-engine)
- [Exposing C++ to Blueprints](https://dev.epicgames.com/documentation/unreal-engine/exposing-cplusplus-to-blueprints-visual-scripting-in-unreal-engine)

Unreal MCP and its toolsets are experimental in Unreal Engine 5.8. Re-check the installed engine source and current Epic documentation when changing engine versions or relying on new toolsets.
