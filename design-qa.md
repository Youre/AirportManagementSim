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

# Empty Network Runway Placement Design QA

**Status:** Passed for internal packaged validation
**Reviewed:** 2026-08-02

> Superseded later on 2026-08-02 by the Starter Network Context correction.
> The empty-parcel, labeled-handle, single-taxi-segment, and mandatory direct
> road observations below describe the prior candidate, not the current build.

## Equivalent-state assessment

- Reference: VA-02 build mode.
- Runtime state: an empty Riverbend starter parcel before its first airfield.
- Package: `Shipping-20260802-runway-overhaul-final` at 1600 x 900 windowed.

VA-02 remains the visual composition reference while the runtime state remains
truthful to Phase 1. The player receives a dominant parcel, a small categorized
tool palette, concise live evidence, and one clear commit tray. No mature-
airport closure or extension controls are copied into the starter flow.

## Final visual check

| Area | Result | Observation |
| --- | --- | --- |
| Empty-state truth | Pass | The parcel opens with only its boundary, planning grid, and south access gate; no airfield element is pre-placed. |
| Path affordance | Pass | Runway and taxiway use persistent, labeled circular START/END handles and reveal the full surface only after endpoint commitment. |
| Geometry | Pass | Diagonal runway and vertical taxiway surfaces match pointer distance and preserve their aspect at 16:9. |
| Crossing | Pass | The taxiway remains continuous through the runway and gains an amber, text-backed RUNWAY CROSSING marker. |
| Evidence | Pass | The inspector derives `05/23`, 816 m x 20 m, placement count, connection state, and cost from the authored proposal. |
| World result | Pass | Commit replaces the proposal with the same diagonal runway, crossing taxiway, stand, hut, road, and reciprocal runway labels. |
| Component language | Pass | Rounded navy panels, cyan committed geometry, amber non-error crossing evidence, green readiness, concise copy, and visible cursor follow the Riverbend baseline. |

## Bounded correction

The final packaged inspection corrected the south-gate location, hover ghost
occlusion, vertical/diagonal segment scaling, duplicate progress copy, and
camera-facing world labels. The recaptured state has no actionable P0-P2
defect for the requested placement overhaul.

## Intentional differences and remaining debt

- Phase 1 supports one runway and one taxiway segment; later construction
  breadth can add multi-segment networks without changing this interaction
  language.
- The access road is a direct stand-to-south-gate connection in this slice.
- Native UMG/Paper2D surfaces remain less illustrated than VA-02; richer
  project-owned markings, props, and construction thumbnails remain P3 art
  production debt.

Final result: passed.

# Starter Network Context Design QA

**Status:** Passed for packaged interaction validation
**Reviewed:** 2026-08-02

## Current equivalent-state assessment

- Reference: VA-02 build mode.
- Runtime state: a starter parcel with a fixed basic operations terminal and
  Gate A/Gate B anchors, before player-authored runway and taxi construction.
- Development package:
  `Development-20260802-network-context` at 1600 x 900 windowed.

The current state keeps VA-02's map-dominant planning hierarchy without
pretending the Phase 1 airport has the reference image's mature facilities.
The fixed terminal and gates are spatial instructions; the player still draws
the runway, every taxiway segment, and any optional road.

## Final visual and interaction check

| Area | Result | Observation |
| --- | --- | --- |
| Context | Pass | The terminal shell and two gate pads are visible, named, and fixed while editable infrastructure remains absent. |
| Handles | Pass | Runway, taxiway, and road endpoints use compact circles without `START`/`END` labels. |
| Taxi targets | Pass | Taxi mode highlights the runway, Gate A, Gate B, and existing taxiway surfaces using color plus outline/callout treatment. |
| Network truth | Pass | A two-segment route from runway through another taxiway to Gate A changes the inspector to `NETWORK READY`; partial and orphan routes receive localized recovery copy. |
| Road meaning | Pass | Service-road mode says it is optional, can follow any route, and has no required connection; the inspector explains the walking baseline and 20% useful-road benefit. |
| World result | Pass | Commit preserves the horizontal `09/27` runway, two taxi segments, Gate A destination, terminal context, and disconnected optional road. |
| Component language | Pass | Rounded navy rails, cyan authored geometry, amber crossings, coral recovery, green readiness, concise uppercase labels, and one primary commit action retain the Riverbend baseline. |

## Remaining visual debt

- The current terminal and gate world sprites are deliberately schematic and
  smaller than the mature-site illustration in VA-02.
- Construction thumbnails, richer terminal/gate props, pavement markings, and
  world detail remain P3 production-art opportunities; they do not obscure
  the placement instructions or invalidate the current interaction model.

Final result: passed.

# Phase 7 Build-mode UX Completion Design QA

**Status:** Passed for the starter-airfield build-mode correction
**Reviewed:** 2026-08-02

## Source and implementation

- Source: `docs/planning/30-content-and-assets/concept-art/02-build-mode-runway-extension.png` (VA-02)
- Implementation: `AMSim/Source/AMSimUI/Private/AMSimConstructionProposalView.cpp`
- Root integration: `AMSim/Source/AMSimUI/Private/AMSimRootScreen.cpp`
- Viewport: 1920 x 1080 at 100% UI scale
- Capture: `AMSim/Saved/Phase7/BuildModeUXCompletion-20260802/implementation-build-mode-1920x1080.png`
- Combined comparison: `AMSim/Saved/Phase7/BuildModeUXCompletion-20260802/comparison-va02-vs-build-mode.png`

The states are equivalent in interaction purpose, not airport maturity: VA-02
is a runway-extension proposal at an operating airport; the implementation is
the first complete starter-airfield proposal on an empty parcel. Closure,
capability, and large facility-catalog details are therefore intentional
differences rather than missing Phase 1 behavior.

## Senior visual check

| Area | Result | Observation |
| --- | --- | --- |
| Layout | Pass | The map is dominant, normal gameplay rails are absent, persistent header remains, and bounded left tools/right evidence/bottom decision regions follow VA-02's composition. |
| Components | Pass | Rounded navy surfaces, cyan selected states, green/coral validation, concise uppercase labels, and a large positive commit action use the Riverbend component language. |
| Placement truth | Pass | The visible parcel, 10 m grid, endpoints, handles, dimensions, connection chain, and status card are derived from the proposal and authoritative validator. |
| Invalid state | Pass | Coral handle outlines and offset text callouts identify the exact broken locations without relying on color; confirmation is disabled until recovery. |
| Interaction | Pass | Direct map placement, two-click paths, handle editing, reset, undo, Escape/cancel, confirmation, and visible cursor were replayed in Shipping. |
| Density | Pass with P3 debt | The starter stand/road/hut cluster is denser and less illustrated than VA-02, but the correction removes the former nudge-panel clutter and leaves all controls readable and operable. |
| Responsive baseline | Pass | The default 1600 x 900 window and maximized 1920 x 1080 state both retain the tool, parcel, validation, and decision regions without cropped critical actions. |

## Iteration history

1. Replaced nudge controls and constrained preview with direct full-map mouse
   placement, but the first packaged correction still exposed underlying black
   side areas, overlapped the parcel label, and overemphasized status surfaces.
2. Added intentional navy build-mode backdrops, moved the parcel label into the
   parcel, converted validation to a dark state-outlined inspector, shortened
   the rails, separated clustered handles/diagnostics, and derived the runway
   overlay label from actual geometry.
3. Final packaged review caught a wrapped cost suffix; the commit action now
   uses concise no-wrap copy and remains one line at both reviewed sizes.
4. Final comparison found no actionable P0-P2 visual defect in the build-mode
   correction. Remaining illustration/icon density is recorded as P3 production
   debt rather than falsely claiming full concept-art parity.

## Severity result

- P0: 0
- P1: 0
- P2: 0
- P3: project-owned construction thumbnails, richer starter-site props, and a
  later art pass for the stand/service label cluster.

final result: passed
