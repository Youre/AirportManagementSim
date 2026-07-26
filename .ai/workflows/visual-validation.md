# Visual Validation Workflow

Use this workflow whenever UI or world presentation is created, changed, or reviewed against concept art.

1. Select the mapped concept-art reference and name the exact gameplay state, resolution, UI scale, and zoom being compared.
2. Inspect the reference before implementation. Record its hierarchy, world/UI balance, palette, surface language, typography, spacing, and operational-state cues.
3. Capture the implementation at the equivalent state through Unreal MCP or the deterministic screenshot fixture.
4. Run `scripts/ui/New-VisualComparison.ps1` to create a side-by-side comparison board and review worksheet.
5. Inspect the board as an image. Record the three highest-impact mismatches, intentional differences, and accessibility or gameplay constraints that override the art.
6. Choose one bounded visual system per iteration, such as primary actions, panel chrome, typography, world textures, or status treatment.
7. Mutate through the appropriate boundary: Unreal MCP for UMG/assets/editor state; source tools for native code and configuration.
8. Compile, save, re-read, and recapture the same state. Do not substitute a different state that hides the mismatch.
9. Repeat until the bounded target is accepted or its remaining differences are explicitly deferred.
10. Run interaction checks after visual changes: cursor visibility, clicking, hovering, focus, supported UI scales, captions, and non-color status encoding.

Screenshot existence, non-black pixels, and layout-overlap tests are necessary but do not constitute visual approval. Concept art is directional; written gameplay, accessibility, 2D, and information requirements remain authoritative.

Example:

```powershell
.\scripts\ui\New-VisualComparison.ps1 `
  -ReferencePath .\docs\planning\30-content-and-assets\concept-art\01-airport-overview-hud.png `
  -CapturePath .\AMSim\Saved\Phase1\va01-new-airport.png `
  -OutputPath .\AMSim\Saved\VisualReview\VA-01-comparison.png `
  -ReferenceId VA-01 `
  -StateDescription 'New temperate airport at 1920x1080, 100% UI scale'
```
