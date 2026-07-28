# Phase 6 Visual Validation

**Status:** Technical baseline reviewed; final owner visual acceptance open
**Reviewed:** 2026-07-28
**Visual direction:** Riverbend rounded UI, strict top-down Paper2D, River &
Sun 787-9

The written requirements, VA-01 through VA-07, and these repository exports
are authoritative. The existing Figma file could be opened but not edited in
the available unauthenticated browser session, so no Figma node IDs are
claimed for Phase 6. `board-source.html` remains the editable source and
`ExportPhase6VisualBoards.ps1` reproduces the five required 1920 × 1080
equivalent states plus the supplemental maximum-scale frame.

| Frame | Export | References | Bounded correction |
| --- | --- | --- | --- |
| P6-01 Parallel runway operations | [PNG](p6-01-parallel-runway-operations.png) | VA-01, VA-04 | Unified correct reciprocal labels, runway roles, crossing protection, and timetable impact |
| P6-02T High-capacity terminal flow | [PNG](p6-02-high-capacity-terminal-flow.png) | VA-03 | Split passenger, baggage, gate, and public-access capacity into connected world regions |
| P6-02 Wide-body turnaround | [PNG](p6-02-widebody-turnaround.png) | VA-05 | Protected true-scale aircraft envelope and promoted the next ordinary-dispatch dependency |
| P6-03 Major capability | [PNG](p6-03-major-capability.png) | VA-07 | Replaced exclusive branches with six equal evidence paths and earned/operational bands |
| P6-04 Serious incident continuity | [PNG](p6-04-serious-incident-continuity.png) | VA-06 | Localized the closure and made adjacent-runway continuity explicit |
| P6-05 Maximum scale | [PNG](p6-05-maximum-scale-overview.png) | VA-01, VA-03 | Grouped 10,000 logical agents into readable regions while retaining exact pool counts |

Each frame keeps a 64% continuous world region, concise side rails, rounded
component language, visible-mouse expectation, and text-plus-shape status
encoding. The generated comparison boards and complete mismatch notes live in
[`comparisons/`](comparisons/).

The River & Sun aircraft direction is owner-approved. The five PI-09
equivalent states plus the supplemental maximum-scale frame have completed
repository-side comparison and correction review, but still require the
explicit owner visual checkpoint recorded by PI-09.

## Aircraft evidence

- [Owner approval record](787-scale-livery-review.md)
- [Scale and livery board](787-scale-livery-review-candidate.png)
- [Sixteen-heading review board](../../../../SourceAssets/Phase6/Aircraft/Riverbend_Longreach_787-9_heading-review.png)

The small `00R` label in the earlier generative approval board remains a
documented board-only artifact. All Phase 6 validation frames, simulation
state, and automated tests use `09R/27L`.
