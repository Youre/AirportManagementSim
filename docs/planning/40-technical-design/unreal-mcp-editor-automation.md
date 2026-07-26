# TS-07: Unreal MCP and Editor Automation

**Status:** Approved baseline  
**Owner:** Editor tooling  
**Last updated:** 2026-07-25

## Outcome and scope

Unreal MCP and scripted editor tools accelerate inspection, content creation, validation, and repeatable setup without becoming runtime dependencies or uncontrolled mutation paths.

## Boundary

ModelContextProtocol, AllToolsets/ToolsetRegistry, PythonScriptPlugin, RemoteControl, and custom editor commands are development-only. They are disabled or excluded in Shipping builds and never required to open a save or play the game. The game has no runtime MCP endpoint.

The official Unreal MCP plugin is experimental and operates through the editor. Its server is bound to localhost only. It must not be exposed to LAN/WAN, packaged builds, or untrusted clients.

## Approved tool categories

- read project/plugin/config and asset metadata;
- inspect levels, assets, Blueprints, widgets, data registries, gameplay tags, and validation results;
- create assets from approved templates;
- apply documented Paper 2D import presets;
- run content validators and automation tests;
- capture screenshots and structured reports;
- execute idempotent project-setup actions;
- invoke approved Python/editor utility scripts.

MCP must not copy aircraft files from `C:\Users\dave\Documents\Joes_Game\dist\assets` until CT-01's manifest is approved, alter source-control history, delete asset trees, install third-party runtime plugins, change security/network exposure, or perform broad refactors without explicit task scope.

## Tool contract

Every mutating custom editor tool declares input schema, dry-run capability where meaningful, output report, affected asset paths, idempotency behavior, transaction/undo support where Unreal permits, and validation performed. Bulk operations begin with discovery and preview. Generated assets use deterministic names from stable content IDs.

Long operations report progress and can stop between assets without corrupting completed work. Re-running after interruption must skip verified identical output or repair only incomplete items.

## Planned toolsets

1. project baseline and plugin/config audit;
2. sprite intake, slicing, pivot, scale, direction, and atlas preparation;
3. definition/table/registry creation and reference validation;
4. map/network/facility inspection;
5. save fixture generation and migration testing;
6. headless scenario and performance fixture execution;
7. screenshot and documentation evidence capture.

For visual work, screenshot capture is followed by the repository visual-validation workflow: equivalent-state comparison, image inspection, prioritized mismatch notes, one bounded mutation, and same-state recapture. MCP may author and inspect the editor asset, but it cannot approve its own visual result.

## Verification and audit

Tool invocations write a local developer log containing tool/version, time, inputs without secrets, affected paths, result, and validation summary. A source-control diff remains the final authority. MCP output cannot mark its own result approved without independent validator/test evidence.

## Acceptance and sources

Acceptance requires a documented localhost editor setup, a read-only project audit, one idempotent sprite-import dry run, a validator invocation, test execution, interruption recovery, and proof that a Shipping package contains no MCP endpoint or editor modules.

Sources: [Unreal MCP in Unreal Editor](https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor), [Scripting the Unreal Editor using Python](https://dev.epicgames.com/documentation/unreal-engine/scripting-the-unreal-editor-using-python).
