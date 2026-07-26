# Phase 4 Design QA

**Status:** Passed
**Reviewed:** 2026-07-26

## Source and implementation

- VA-04 source:
  `docs/planning/30-content-and-assets/concept-art/04-flight-planning-timetable.png`
- VA-06 source:
  `docs/planning/30-content-and-assets/concept-art/06-weather-incident-response.png`
- UI implementation:
  `AMSim/Source/AMSimUI/Private/AMSimRegionalOperationsView.cpp`
- Paper2D implementation:
  `AMSim/Source/AMSimUI/Private/AMSimWorldPresenter.cpp`
- View-state implementation:
  `AMSim/Source/AMSimUI/Private/AMSimPhase4ViewState.cpp`

## Equivalent states and capture contract

| State | Viewport | Density | Capture | Combined full-view comparison |
| --- | --- | --- | --- | --- |
| Published 21-flight regional week with three accepted recurring operators and selected high-risk flight | 1920 x 1080 | 100% | `AMSim/Saved/Phase4/va04-regional-timetable.png` | `AMSim/Saved/Phase4/Comparisons/va04-phase4-comparison.png` |
| Warned Runway 27 incident after hold, divert, and tow dispatch, before area protection | 1920 x 1080 | 100% | `AMSim/Saved/Phase4/va06-regional-incident.png` | `AMSim/Saved/Phase4/Comparisons/va06-phase4-comparison.png` |

Focused high-density regions were inspected separately at 175% and 200%:
top status bar, contract cards, day tabs/cards, selected-flight evidence,
weather cards, incident lifecycle cards, cause/remedy tray, and footer
actions. The same deterministic states were recaptured after correction.

## Senior visual check

| Area | Result | Observation |
| --- | --- | --- |
| Typography | Pass | Off-white operational type, cyan hierarchy, amber caution, coral closure, and short compact labels are coherent and legible. |
| Layout | Pass | VA-04's contract/timetable/evidence hierarchy and VA-06's weather/world/incident/cause hierarchy are preserved. |
| Components | Pass | Rounded panels, raised cards, status surfaces, and primary/secondary/destructive/positive actions reuse the accepted Phase 1.5 theme. |
| State truth | Pass | All visible values are derived from Phase 4 query/state; concept-art values are not copied. |
| Interaction | Pass | Every Phase 4 mutation is player-facing; time, save/load, warning confirmation, incident response, renewal, and visible-mouse policy remain active. |
| Responsive behavior | Pass | 100-150% retains seven-day rails. 175-200% presents one day's three cards, bounded scrolling rails, compact data-derived evidence, and a taller footer. |
| Assets | Pass with intentional residual | The approved schematic Paper2D world replaces the concept's bespoke illustrated airport and vehicle density; no fake or unapproved asset was added. |

## Findings and iteration history

1. First capture: timetable structure matched VA-04, but cards omitted aircraft
   class and the evidence rail carried too much prose.
2. First incident capture: structure matched VA-06, but weather/headline/
   consequence copy clipped in narrow cards.
3. First 200% review: automated capture passed, but visual inspection found
   brand, evidence, lifecycle, and footer clipping.
4. Bounded correction: added aircraft codes to flight cards, shortened
   operational copy, added compact values derived from real state, shortened
   the compact brand, and expanded the footer.
5. Final recapture: 100%, 175%, and 200% boards/screens have no P0-P2 visual
   defect; the remaining illustrated-art density gap is intentional and
   isolated to later production art.

## Severity result

- P0: 0
- P1: 0
- P2: 0
- P3: one future art-production opportunity for richer project-owned
  terminal/runway props, operator marks, aircraft, response vehicles, and
  icons without changing the accepted layout or simulation contract.

final result: passed
