<!-- ai-project-init:begin managed -->
# UI Templates

Record reusable UI templates here.

Each template should include:
- purpose
- applicable screens or flows
- visual rules
- component or file location
- accepted placeholder content

Template-first rule:
- Check this file before creating UI components.
- If an applicable template exists, use it.
- If no applicable template exists, add a new template and then implement from it.
<!-- ai-project-init:end managed -->

## Primary action button

- Purpose: high-confidence actions that advance the current Phase 1 objective.
- Applicable screens or flows: airport creation, build commitment, opening,
  offer acceptance, and scheduling.
- Visual rules: 12 Slate-unit rounded corners; deep navy-blue normal surface;
  brighter blue hover surface; 2-unit cyan outline; off-white bold label;
  generous content padding; verb-plus-object text that remains independently
  accessible.
- Component: `AMSim::UITheme::ButtonStyle(EButton::Primary)`;
  `/Game/UI/WBP_PrimaryActionButton` remains the cooker-visible Blueprint proof
  asset.
- Accepted placeholder content: `CONTINUE`.

## Riverbend component language

- Purpose: provide one reusable production language rather than styling each
  screen independently.
- Applicable screens or flows: every gameplay HUD, management panel, modal,
  component gallery, and future Phase 2 extension.
- Source: `AMSim/Source/AMSimUI/Public/AMSimUITheme.h` and
  `AMSim/Source/AMSimUI/Private/AMSimUITheme.cpp`.
- Palette: near-black navy chrome; dark-blue panel and card layers; cyan focus
  and selection; amber timing/cost/caution; green confirmation; coral
  destructive action; warm off-white primary text; blue-gray secondary text.
- Geometry: 18-unit outer chrome/panels, 14–16-unit cards, and 10–12-unit
  fields, chips, and buttons. Every production surface uses a rounded-box brush
  with a visible outline; default rectangular Slate brushes are not acceptable.
- Typography: bold display names and current state, compact uppercase eyebrow
  labels, regular sentence-case supporting text, and restrained text shadow
  for hierarchy over the world.
- Density: show one active objective, one short status, one contextual decision
  surface, and only commands valid for the current phase. Routine diagnostic,
  ledger, compatibility, and service detail must not occupy permanent rails.
- Interaction variants: primary, positive, secondary, destructive, tool, and
  quiet buttons share geometry while retaining distinct fill/outline states.
- Reference gallery: `/Game/Developer/Phase15/WBP_AMSimComponentGallery`.

## Phase 1.5 HUD region shell

- Purpose: preserve a world-dominant gameplay surface while assigning stable
  jobs to persistent UI regions.
- Applicable screens or flows: all gameplay states from empty parcel through
  first-flight completion.
- Visual rules: persistent segmented top status; compact left tool rail; right
  objective/evidence cards; transparent center; rounded contextual decision
  tray; slim caption/control footer; compact drawers at 175–200% scale.
- Component: `/Game/UI/Screens/WBP_AMSimRootScreen`, backed by
  `UAMSimRootScreen`.
- Accepted placeholder content: `Test objective`, `No active item`, and `Ready`.

## Airport naming card

- Purpose: name a newly acquired parcel without turning first use into a
  full-width form banner.
- Applicable screens or flows: the initial airport-creation state.
- Composition: one bounded card centered above the lower map edge; parcel
  identity on the left, a wider labeled name field in the middle, and one
  vertically centered primary action on the right. All three regions share one
  visual centerline and the card uses only the height required by two text rows.
- Responsive behavior: retain the three-region row at 100-150% on a 1920x1080
  viewport. At 175-200%, or in windows below 1760x990, use a centered vertical
  card with the concise parcel identity, field, and action. The card must not
  span the map or obscure the central planning area.
- Component: `UAMSimRootScreen::CreateAirportTray`, with bounds supplied by
  `FPhase1HudPresentation::MakeAirportCreationLayout`.
- Accepted placeholder content: `RIVERBEND PLAINS`, `Riverbend Field`, and
  `CREATE AIRPORT`.

## Compact expanding tool rail

- Purpose: keep persistent navigation discoverable without permanently
  consuming world width or repeating long labels.
- Applicable screens or flows: the normal gameplay left rail and future
  map-dominant tool palettes that contain stable destinations rather than form
  controls.
- Composition: a 112-unit rounded rail overlays the map edge; each project-owned
  icon sits in an 88-by-56 button. Only the hovered or keyboard-focused
  item reveals a rounded text flyout to the right. The flyout overlays the world
  and never resizes or shifts it.
- Interaction: icon, hover/focus treatment, tooltip, and accessible label all
  use the same concise name. Hover expansion is immediate and stable; focus is
  visibly retained; disabled tools remain recognizable but cannot emit a
  success cue. Click remains the only action trigger.
- Visual rules: use the existing cooker-visible Riverbend icon kit; do not use
  emoji, abbreviations, or mixed icon families. Icons are 38-42 units, labels
  retain their authored aspect ratio inside that square, and labels use compact
  uppercase text inside a minimum 156-unit flyout. The widget host must include
  the translated flyout without intercepting the world beneath its transparent
  area. Selected/available/disabled states use the shared tool-button style
  plus text-independent contrast.
- Component: `UAMSimExpandingToolButton`, hosted by `UAMSimRootScreen`.
- Accepted placeholder content: `BUILD`, `SCHEDULE`, and `OVERLAYS`.

## Compact map activity card

- Purpose: communicate one active world operation without covering the world
  or repeating information already visible in persistent rails.
- Applicable screens or flows: starter-airfield construction and later single
  active-project summaries.
- Composition: one small rounded card in the upper-right map corner, clear of
  the compact tool rail and objective rail. It has one stage eyebrow and one identity/economy line;
  nested cards and side-by-side evidence boxes are prohibited.
- Content: construction uses `CONSTRUCTION  •  <STAGE>` and
  `RWY <DESIGNATORS>  •  <COST>`. Taxiway/gate membership, package title, and
  explanatory prose stay in contextual inspection or world cues unless they
  are the current blocker.
- Responsive behavior: retain two concise lines at 100-150%; allow wrapping to
  three lines at 175-200% without shrinking type. Status remains explicit and
  never depends on amber/cyan alone.
- Component: the compact state of `UAMSimRootScreen::ContextPanel`.
- Accepted placeholder content: `CONSTRUCTION  •  BUILDING` and
  `RWY 09/27  •  3,400 CR`.

## Evidence card

- Purpose: pair an operational result with cause and corrective action without
  exposing debug state.
- Applicable screens or flows: construction validity, offer compatibility,
  service readiness, recovery, and warnings.
- Visual rules: rounded dark navy surface; short eyebrow heading; status text;
  cause and remedy use separate labeled rows only when a constraint exists.
- Component: reusable native card content hosted by the root Widget Blueprint
  regions.
- Accepted placeholder content: `Valid`, `No active constraint`, and
  `Choose an available action`.

## Service task card

- Purpose: show automatic work, dependency, progress, and blocking state.
- Applicable screens or flows: Phase 1 inspection and fueling, extended by
  later turnaround systems.
- Visual rules: identity, verb-plus-object label, status text, patterned
  progress, and dependency connector; color never acts alone. Automatic
  dispatch must be explicit, world equipment must occupy safe authored
  positions, and player priority/assignment must never be phrased as manual
  vehicle steering.
- Component: reusable native card content hosted by the contextual decision
  region.

## Construction lifecycle overlay

- Purpose: keep a committed project spatially understandable after the proposal
  closes.
- Applicable screens or flows: funded, awaiting-delivery, building,
  inspection, ready, operational, cancellation, and refund states.
- Visual rules: retain the authoritative footprint; pair stage tint with
  project-owned vehicle/worker/cone or inspection art; keep unaffected
  operations open; clear every work proxy when the authoritative stage returns
  to none or completes. Never invent a closure on an unopened site.
- Component: `UAMSimConstructionProposalView` and
  `AAMSimWorldPresenter::RefreshPhase1OperationsPresentation`.

## Construction proposal editor

- Purpose: let a first-time player assemble and repair a starter airfield by
  reasoning from the map before any authoritative construction command is sent.
- Applicable screens or flows: starter-airfield proposal and later
  construction planning surfaces that reuse its direct-manipulation language.
- Composition: keep the persistent airport/time/funds header; replace normal
  gameplay rails and the footer with one categorized tool palette, one concise
  validation rail, a dominant owned-parcel map, and a single cancel/commit tray.
- World cues: begin with an empty aircraft-movement network but retain the
  map-authored basic operations terminal and its two preset gate ports. Never
  pre-lay a runway, taxiway, or player road. Show the parcel boundary, subdued
  major planning grid, cyan patterned committed proposal geometry, compact
  endpoint circles, snap targets, and coral patterned diagnostics attached to
  the exact invalid point or segment. The map must explain every validation
  message.
- Interaction: runway, taxiway, and road tools use compact unlabeled endpoint
  circles. The first press fixes one circle; pointer movement draws only a thin
  centerline to the second; the second press or drag release commits the path.
  Completed endpoints remain draggable. Taxi mode highlights compatible gates,
  the runway surface, and existing taxi segments, and allows repeated segments
  to form one graph. Selected geometry follows a 10 m grid; reset restores the
  map-authored terminal with no player network; undo, cancel, and Escape remain
  recoverable and never mutate simulation.
- Crossing behavior: a taxiway may meet or cross a runway at any point. Mark a
  detected crossing with a symbol and `RUNWAY CROSSING` text; do not reject or
  visually clip the taxiway merely because its centerline intersects the runway.
- Network behavior: readiness requires the complete proposed taxi graph to
  connect a runway to at least one terminal gate. Joins through other taxiways
  are valid; an orphan proposed segment remains a localized blocker. Roads are
  optional and communicate worker travel benefit rather than a stand-connection
  requirement.
- Evidence: after a runway is committed, derive and draw both runway numbers
  from its actual map heading and show its actual heading/length/width. Before
  commit, show only endpoint instructions and live length. Also show selected-
  item geometry, cost, connectivity, surface/workforce summary, and capability
  gain. Keep copy concise and use color plus pattern, border, handle, and text.
- Component: `UAMSimConstructionProposalView`, hosted by
  `UAMSimRootScreen` while normal gameplay chrome is temporarily collapsed.
- Accepted placeholder content: `Select a tool`, `No placement issue`, and
  `Connect the highlighted endpoints`.

## Phase 3 terminal flow surface

- Purpose: present one complete domestic terminal journey without replacing
  the shared gameplay shell or hiding the airport world.
- Applicable screens or flows: terminal construction, route validation,
  passenger departure/arrival, baggage, domestic security, and landside
  access.
- Source visual:
  `docs/planning/30-content-and-assets/concept-art/03-terminal-passenger-flow.png`.
- Composition: persistent top status; compact terminal tool rail; dominant
  top-down cutaway; named-party rail; bottom overlay legend. At 175-200%,
  tools and party details become compact drawers while the terminal remains
  the largest continuous region.
- World palette: warm neutral floors; green landside route; cyan sterile
  boundary and departure route; purple arrivals/reclaim route; amber
  congestion hatch; yellow dashed accessible route.
- World encoding: every route uses direction plus a distinct pattern or
  symbol and a text legend. Secure transitions use shield/checkpoint symbols;
  accessible links use the accessibility label and dashed route; congestion
  uses hatch plus label.
- Selected-party card: identity and member count, flight, current step,
  compact needs, route, and time confidence. Do not show internal IDs, full
  ledgers, or more than one short cause/remedy at once.
- Component: `UAMSimTerminalView`, mapped from `FPhase3QuerySnapshot`.
- Accepted placeholder content: `Maya's party`, `Security`,
  `Accessible route ready`, and `On time`.

## Phase 4 regional operations surfaces

- Purpose: extend the shared gameplay shell with a scanable seven-day timetable
  and a recoverable weather/incident command surface.
- Applicable screens or flows: recurring contract acceptance, exact slot/gate
  ownership, selected-flight feasibility, connection recovery, international
  processing, weather restrictions, incident response, and tenant renewal.
- Source visuals:
  `docs/planning/30-content-and-assets/concept-art/04-flight-planning-timetable.png`
  and
  `docs/planning/30-content-and-assets/concept-art/06-weather-incident-response.png`.
- Timetable composition: compact contract rail; dominant day/time/gate grid;
  selected-flight and feasibility rail; one bounded primary action. Cards use
  short operator, flight, aircraft, exact time, gate, and icon-plus-text risk.
- Incident composition: current/forecast weather rail; dominant operational
  world with localized route and closure; lifecycle rail; captioned
  cause/consequence/action tray. Continued operation outside the closure is
  stated and visible.
- State encoding: cyan selected; green ready/complete; amber patterned
  high-risk; coral patterned blocked/critical; blue-gray waiting. Color is
  always paired with text, border treatment, or pattern.
- Operational modification closures use a localized patterned world hatch on
  the precise affected segment and explicitly name the runway, stand, or route
  that remains open. Do not dim or recolor an entire operational site.
- Timetable risk overlays use the complete `WX CONFLICT ///` or
  `LOCK RISK ///` label with the authoritative time window and flight identity;
  a compact card status alone is insufficient.
- Responsive behavior: 100-150% retains both rails; 175-200% converts contract,
  feasibility, weather, and lifecycle rails into bounded drawers without
  shrinking the timetable or operational labels below the minimum.
- Component: `UAMSimRegionalOperationsView`, mapped from
  `FPhase4QuerySnapshot`.
- Accepted placeholder content: `No regional timetable`, `Select a flight`,
  `Forecast available`, and `No active incident`.

## Capability path inspector

- Purpose: make non-exclusive airport specialization understandable as
  capability evidence rather than a character class.
- Applicable screens or flows: airport capabilities, suggested objectives, and
  future-locked path inspection.
- Visual rules: six equally prominent project-owned thumbnails; explicit
  Established, Regional, Advanced, and Major bands; exactly three suggested
  objectives; selected-path requirements, current evidence, spatial footprint,
  business effect, next capability, and reward/outcome. State that paths can be
  combined and grant no artificial class bonus.
- Component: `UAMSimProgressionView`, mapped from
  `FProgressionViewState`.
- Accepted placeholder content: `Future locked`, `No evidence yet`, and
  `Paths can be combined`.

## Phase 5 cargo, provider, and event surfaces

- Purpose: extend the Riverbend component system without turning cargo or
  special events into text-only dashboards.
- Cargo contract card: identity, direction, class, pieces/mass/volume,
  deadline, compatibility, concise exception/remedy, and one primary action.
- Warehouse card: zone type, occupied/capacity values, compatibility label,
  road/airside connectivity, and color-independent full/blocked state.
- Cargo-flow inspector: road receipt, check/security, compatible storage,
  build-up/breakdown, airside handling, aircraft, and completion. The same
  locations and route must appear in the Paper2D world.
- Provider/concession card: footprint, opening cost, rent or revenue share,
  required capability, service expectation, operating pattern, satisfaction
  drivers, term, and explicit grace/recovery state.
- Event card: preview, preparation requirements, ordinary linked demand,
  active status, partial outcome, cleanup, reward, and cooldown. State what
  remains open during any localized closure.
- Responsive behavior: retain at least 60% continuous world area at 100-150%;
  convert side rails to bounded compact drawers at 175-200%.
- Concept references: VA-01, VA-03, VA-04, VA-05, VA-06, and VA-07.
- Components: `UAMSimPhase5View`,
  `AAMSimWorldPresenter::RefreshPhase5Presentation`, and
  `FPhase5ViewState`.

## Release guidance and accessibility surface

- Purpose: keep first-use help, the offline manual, authentic-term glossary,
  controls, and accessibility choices available without leaving the game.
- Applicable screens or flows: every gameplay phase, contextual first-use
  guidance, accessibility review, and the formal unassisted tester journey.
- Source visuals: VA-01 for peripheral guidance and world dominance, VA-06 for
  captions and cause/consequence/action, and VA-07 for approachable progress
  cards.
- Composition: full-screen navy modal above the live airport; compact
  icon-plus-label section tabs; one dominant readable content panel; one
  optional quick-reference rail at 100-150%; compact single-panel mode at
  175-200%; persistent close action.
- Cards: rounded 14-18 unit surfaces, one short eyebrow, one plain-language
  explanation, and at most one next action. Aviation terms remain authentic
  and are defined in place.
- Accessibility states: scale choices show selected text plus border/fill;
  captions show explicit `ON`; reduced motion shows `ON/OFF` text; camera
  shake shows `OFF`; no status is color-only.
- Contextual card: no more than 45 words; anchored above the bottom context
  area; `GOT IT` is the only primary action; acknowledgement is save-local.
- Components: `UAMSimReleaseGuideView`, `UAMSimContextHelpCard`,
  `UAMSimAccessibilityProfile`, and the shared Riverbend theme.
- Accepted placeholder content: `Select a section`, `No help needed here`,
  and `Captions on`.

## Riverbend UI audio

- Purpose: reinforce interaction and state without becoming required for
  comprehension or adding screen-specific audio plumbing.
- Every new button must use `AMSim::UITheme::ButtonStyle` or an approved style
  derived from it. This supplies the shared hover cue and the appropriate
  primary, secondary, or compact-tool press cue automatically.
- Play semantic cues through `EAMSimUISound` and `AMSim::UIAudio::Play`; never
  select sounds from display text and never load a runtime asset by string.
- Generic button audio confirms the physical interaction. Semantic audio
  confirms the authoritative outcome after its command or persistence result.
- Hover, focus, snap, alert, and notification cues retain bounded cooldowns.
  Disabled controls do not emit semantic success sounds.
- Text, captions, patterns, borders, and state labels remain authoritative;
  no control, warning, or progression state may depend on sound alone.
- Source masters live in `SourceAssets/Audio/UI`; cooked `USoundWave` assets
  live in `/Game/Audio/UI` and are validated as short, stereo, non-looping UI
  sounds with reviewed per-cue level trims.
