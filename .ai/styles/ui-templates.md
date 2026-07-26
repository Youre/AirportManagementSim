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
- Applicable screens or flows: airport creation, build commitment, opening, offer acceptance, and scheduling.
- Visual rules: 12 Slate-unit rounded corners; deep navy-blue normal surface; brighter blue hover surface; 2-unit cyan outline increased to 3 on hover; off-white bold 18-point label; 18 x 12 content padding; text must use a verb plus object and remain independently accessible.
- Component: `/Game/UI/WBP_PrimaryActionButton`; initial implementation is the `Create airport` action in `UAMSimRootScreen`.
- Accepted placeholder content: `CONTINUE`.

## Phase 1.5 HUD region shell

- Purpose: preserve a world-dominant gameplay surface while assigning stable jobs to persistent UI regions.
- Applicable screens or flows: all gameplay states from empty parcel through first-flight completion.
- Visual rules: persistent top status; left tool/offer rail; right objective/evidence rail; transparent center; bottom context/caption region; compact drawers at 175–200% scale.
- Component: `/Game/UI/Screens/WBP_AMSimRootScreen`, backed by `UAMSimRootScreen`.
- Accepted placeholder content: `Test objective`, `No active item`, and `Ready`.

## Evidence card

- Purpose: pair an operational result with cause and corrective action without exposing debug state.
- Applicable screens or flows: construction validity, offer compatibility, service readiness, recovery, and warnings.
- Visual rules: deep navy surface; short section heading; icon/pattern plus status text; cause and remedy use separate labeled rows.
- Component: reusable native card content hosted by the root Widget Blueprint regions.
- Accepted placeholder content: `Valid`, `No active constraint`, and `Choose an available action`.

## Service task card

- Purpose: show automatic work, dependency, progress, and blocking state.
- Applicable screens or flows: Phase 1 inspection and fueling, extended by later turnaround systems.
- Visual rules: identity icon, verb-plus-object label, status text, patterned progress, and dependency connector; color never acts alone.
- Component: reusable native card content hosted by the bottom context region.
- Accepted placeholder content: `Inspect aircraft — waiting`.
