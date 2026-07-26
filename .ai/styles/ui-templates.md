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
  progress, and dependency connector; color never acts alone.
- Component: reusable native card content hosted by the contextual decision
  region.
- Accepted placeholder content: `Inspect aircraft — waiting`.
