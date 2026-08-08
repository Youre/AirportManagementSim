# Growable Terminal Target-Frame Generation Record

**Generated:** 2026-08-08
**Service:** OpenAI built-in ImageGen
**Use case:** `ui-mockup`
**Decision:** Approved by owner on 2026-08-08; design evidence only

## Shared reference inputs

| Role | Path |
| --- | --- |
| Authoritative composition and visual language | `docs/planning/30-content-and-assets/concept-art/03-terminal-passenger-flow.png` |
| Project-owned furniture and surface language | `SourceAssets/Phase4.5/Generated/terminal-content-atlas-alpha.png` |
| Current Unreal/Paper2D implementation constraint reference | `AMSim/Saved/Phase1/va03-terminal-passenger-flow.png` |

## Target 01 — starter GA terminal editor

- Source request ID: `exec-30e5e33c-2155-440a-9aa9-e0717cf0a780`
- Built-in output: 1672 x 941 PNG
- Retained output: `target-01-starter-ga-terminal-editor.png`
- Retained dimensions: 1920 x 1080
- SHA-256: `8e0fd979b3f5f004a76f1d8b73d1df963c3d1cf8746e6e4ff10f70d5cd7dfe7e`
- Post-processing: high-quality bicubic resize to the canonical comparison
  resolution; no compositional or color edits.

### Prompt

```text
Use case: ui-mockup
Asset type: 1920x1080 production target frame for a 2D Unreal airport-management game
Input images: Image 1 is the authoritative VA-03 composition and Riverbend visual-language reference; Image 2 is the existing project-owned terminal furniture and surface style reference; Image 3 is the current implementation and should inform practical Unreal/Paper2D constraints, but should be substantially refined.
Primary request: Create a polished, shippable-looking starter general-aviation terminal in BUILD mode. It is a small furnished terminal integrated into the same airport world, shown as a strict orthographic top-down cutaway after its roof has faded away. The terminal is editable on a subtle one-meter grid.
Scene/backdrop: grass airport world remains visible around the compact terminal; service road and curb at the landside edge; apron and two small airside gate-door connection points at the opposite edge. Do not isolate the terminal on a separate screen.
Terminal contents: entrance doors, compact lobby, information/reception desk, several small seating groups, accessible restroom, staff/service room, storage, vending and water, plants and signage, two clearly readable airside gate doors. Keep rooms compact and believable.
Construction state: one small extension is being built along an exterior edge. Show a translucent valid floor-placement ghost, a localized striped closure, delivery materials, and two moving construction workers. Finished rooms remain open and occupied by a few GA visitors.
UI composition: slim persistent navy top status bar no more than 9% of height; compact rounded navy icon rail on the left with short expandable category labels; dominant continuous world/cutaway at least 60% of the image; small contextual inspector in the lower-right showing price, validity, and local closure impact; compact bottom legend. Use short, legible labels only.
Text (verbatim, no additional prose): "TERMINAL BUILD", "STRUCTURE", "DOORS", "FURNITURE", "AMENITIES", "OPERATIONS", "UNDO", "VALID", "LOCAL CLOSURE", "1 m GRID".
Style/medium: polished 2D top-down game UI and sprite environment, warm cartoon realism, crisp Paper2D readability, rounded Riverbend component language; match the visual quality, density, navy chrome, cyan selection, off-white type, and tactile furniture of Images 1 and 2.
Color palette: warm neutral public floors, muted blue-gray staff floor, cyan selection and valid-placement outline, amber active work, coral blocked only, green landside cues; status meaning must not rely on color alone.
Composition/framing: 16:9 landscape, strict top-down orthographic world with no perspective walls, no isometric camera, no 3D render. Keep surrounding airport context visible and the terminal centered.
Constraints: no real airline branding; no photorealism; no 3D models; no upper floor; no jet bridges; no huge modal; no dense debug text; no text-heavy sidebars; no watermark. The player must immediately understand where to build, what will close, what it costs, and how the terminal connects landside to airside.
```

## Target 02 — grown regional terminal operations

- Source request ID: `exec-a560d55a-0ffe-4b5e-ba11-a9816771c871`
- Built-in output: 1672 x 941 PNG
- Retained output: `target-02-grown-regional-terminal-operations.png`
- Retained dimensions: 1920 x 1080
- SHA-256: `d8f10492d9d7691eb5565a16c94c0852e416fbf279a6e4571822d1499fb7fb60`
- Post-processing: high-quality bicubic resize to the canonical comparison
  resolution; no compositional or color edits.

### Prompt

```text
Use case: ui-mockup
Asset type: 1920x1080 production target frame for a 2D Unreal airport-management game
Input images: Image 1 is the authoritative VA-03 composition, hierarchy, palette, and operational-density reference; Image 2 is the project-owned terminal furniture and surface style reference; Image 3 is the current Unreal implementation and should inform feasible Paper2D behavior while being substantially refined.
Primary request: Create a polished, shippable-looking grown regional passenger terminal in OPERATIONS mode. It must be the natural expanded form of a small editable starter terminal, rendered in the same airport world as a strict orthographic top-down cutaway with the roof faded away.
Scene/backdrop: show surrounding apron with two regional aircraft and Gate A1/Gate A2 at the airside edge; curb, road, bus stop, taxi/drop-off, and parking at the landside edge. The terminal footprint is irregular but grid-built and visually coherent.
Terminal contents: clearly separated entrance/lobby, check-in and bag drop, physical queue lanes, accessible domestic security checkpoint, departure lounge with abundant seating and amenities, two gate podiums and airside doors, arrivals corridor, baggage make-up/sorting, reclaim carousel, information desk, restrooms, staff/service rooms, vending/food point, plants and signage. Dense enough to feel alive but not cluttered.
Operational activity: named passenger parties, staff, bags, baggage carts and a few vehicles move through ordinary systems. Show simultaneous green landside/departure flow, cyan controlled secure route and boundary, purple arrival/reclaim flow, yellow patterned accessible route, and one small amber congestion area. Route lines follow real doors and rooms rather than cutting through walls.
UI composition: slim persistent rounded navy top status bar no more than 9% of height; compact icon-plus-label terminal overlay rail on the left; dominant continuous cutaway at least 60% of screen; rounded named-party inspector on the right; compact bottom legend. Use short, scan-friendly copy.
Text (verbatim, no additional prose): "TERMINAL OPERATIONS", "FLOWS", "ROOMS", "SECURITY", "BAGGAGE", "CLOSURES", "MAYA'S PARTY", "RB 304", "ON TIME", "SECURITY", "ACCESSIBLE", "GATE A1", "GATE A2", "CHECK-IN & BAG DROP", "BAGGAGE RECLAIM".
Style/medium: polished 2D top-down game UI and sprite environment, warm cartoon realism, crisp Paper2D management-zoom readability, rounded Riverbend component language; match Images 1 and 2 for navy chrome, cyan edge highlights, off-white typography, tactile furniture, subtle shadows, and friendly operational detail.
Color palette: warm neutral public floors, cool blue-gray secure/departure rooms, muted purple arrivals/reclaim, green landside flow, cyan secure boundary, amber congestion, yellow accessible pattern; every status also has shape, pattern, icon, or text.
Composition/framing: 16:9 landscape, strict top-down orthographic world; no perspective walls, no isometric camera, no 3D render. Preserve surrounding airport context and keep the terminal as the largest continuous region.
Constraints: no real airline branding; no photorealism; no 3D models; no upper floor; no international processing; no giant modal; no debug ledger; no excessive labels over the world; no watermark. The viewer must immediately understand the full landside-to-airside journey, separate arrival and departure paths, secure boundaries, baggage flow, accessibility, and selected party state.
```
