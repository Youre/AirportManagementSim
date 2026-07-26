# Phase 1.5 Visual Foundation

**Status:** Approved implementation template  
**Approved:** 2026-07-26

## Persistent composition

- The top status bar remains visible in all gameplay states.
- The Paper2D airport world is the largest continuous surface and targets at least 60% of the 1920 x 1080 viewport at 100–150% scale.
- The left rail owns tools or the active offer; the right rail owns objective, compatibility, or cause/remedy evidence.
- The bottom context region owns creation, confirmation, timetable action, selected aircraft, dependencies, captions, and save/load controls.
- At 175–200%, desktop rails collapse into compact contextual drawers; body text never shrinks below the supported minimum.

## Reusable components

| Template | Required states |
| --- | --- |
| Action button | primary, secondary, destructive, disabled, hover, focus |
| Tool button | selected, available, locked |
| Status chip | information, valid, caution, critical, waiting, active, complete |
| Panel frame | persistent rail, contextual card, modal |
| Evidence row | icon/pattern, label, value, status |
| Objective card | objective, cause, corrective action |
| Timetable row | available, selected, unavailable, owned stand |
| Service task card | waiting, active, complete, blocked |
| Caption bar | speaker/category, complete operational meaning |

## State templates

- **New airport:** empty Riverbend Plains world, objective rail, bottom creation context.
- **Construction:** cyan patterned starter-package proposal, exact cost/contingency evidence, bounded confirmation.
- **Offer/timetable:** one offer, one exact five-minute slot, adjacent Stand A1 and compatibility evidence.
- **Turnaround:** aircraft-centered world, inspection and fueling only, automatic task dependency strip.

## Surface treatment

- Generated surface texture is subtle evidence, not the visual anchor.
- Use normalized low-contrast texture over the approved palette.
- Markings, outlines, icons, props, labels, and status patterns remain deterministic and editable.
- No perspective, 3D gameplay asset, real operator brand, or generated incidental gameplay value is permitted.

