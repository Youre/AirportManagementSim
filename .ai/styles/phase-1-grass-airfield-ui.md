# Phase 1 Grass-Airfield UI Template

**Status:** Approved implementation template
**Owner:** UI and visual presentation
**Last updated:** 2026-07-26

## Intent

The first playable surface keeps the 2D airport dominant while construction, flight, and explanation controls remain readable for a child with basic English.

## Layout

- top bar: airport name, credits, Airport Points, clear-weather summary, game time, speed;
- left rail: starter build action and construction explanation;
- right rail: current objective, offer/flight activity, warnings;
- bottom panel: selected runway, stand, project, or aircraft details;
- center world: north-up grass parcel, runway, taxi connection, stand, operations hut, construction activity, and aircraft path;
- modal layer: map/name/slot creation, confirmation, save/load, and blocking failure only.

## Palette

| Token | Value | Use |
| --- | --- | --- |
| `NavyChrome` | `#122433` | management chrome and panel backgrounds |
| `NavyDeep` | `#09141D` | outer frame and modal scrim |
| `Grass` | `#5F8F57` | owned buildable parcel |
| `GrassDark` | `#426B41` | unowned/scenery distinction |
| `RunwayGrass` | `#789A68` | operational grass runway |
| `Taxi` | `#B09A68` | grass taxi route |
| `ConcreteWarm` | `#B9AA8E` | stand and operations hut |
| `TextPrimary` | `#EAF5F8` | primary text |
| `TextSecondary` | `#AFC5CE` | explanatory text |
| `CyanSelection` | `#51D4E8` | selection and proposed-valid outline |
| `AmberCaution` | `#F1B84B` | warning and waiting state |
| `CoralCritical` | `#F06A66` | blocked/critical state |
| `GreenReady` | `#73D08A` | valid, ready, completed state |

## Component rules

- Every button uses a verb plus object, such as `Build starter airfield`.
- A blocking result always displays cause and corrective action.
- Status never relies on color alone; pair color with label, icon-like shape, or pattern.
- Minimum body text matches 18 px at 1080p/100% scale.
- Current objective uses at most two short sentences.
- Radio captions include speaker, call sign, and complete operational meaning.
- Presentation animations cannot trigger authoritative completion.
- The UI may simplify a dense final-game panel, but its commands and query sources must be the stable Phase 1 contracts.

## Visual comparison

- VA-01: world dominance and HUD regions;
- VA-02: proposed versus built geometry and cause/remedy feedback;
- VA-04: offer-to-slot ownership and compatibility evidence;
- VA-05: aircraft-centered service state and dependency explanation.

Exact concept-art geometry, values, typefaces, and dimensional shading are non-normative.
