# Runway Surface and Construction Pacing Adversarial Review

Date: 2026-08-02
Status: Approved with controls below

## Failure modes and controls

1. **Construction still flashes by at 1x.** Tests assert the four new fixture
   thresholds and a build-stage span of 7,200,000 game milliseconds. Packaged
   captures exercise delivery and mid-build rather than skipping directly to
   inspection.
2. **The slowdown makes acceleration unusable.** All thresholds stay in normal
   simulation time, so 2x through 8x and pause/resume behave consistently.
3. **Older saves move backward or corrupt.** No persisted field changes. A
   loaded stage is never reset; only future threshold transitions use the
   current balance fixture.
4. **Generated texture contains fake markings, objects, borders, or
   perspective.** ComfyUI prompts request only unmarked top-down aggregate.
   Markings are deterministic project-authored overlays applied after review.
5. **Seams appear when sprites stretch.** Retained derivatives use mirrored
   edge construction, exact 512x512 output, and a tiled contact-sheet review.
6. **Runway markings repeat nonsensically.** The texture contains one coherent
   longitudinal runway marking system rather than a photographed whole runway.
   Arbitrary geometry continues to scale the continuous surface.
7. **Taxiway markings appear on service roads or brown earthwork.** A separate
   hard-referenced access sprite owns both, and tests assert all required assets
   resolve independently.
8. **The new art accidentally grants paved capability.** This pass changes
   presentation only. Phase 1 facility IDs and compatibility remain the starter
   grass/compacted-airfield definitions until a separately reviewed gameplay
   upgrade changes them.
9. **Numbers are corrected for one direction but broken for diagonals.** The
   pure yaw helper is tested for horizontal, vertical, and reciprocal deltas;
   same-state rendered proof checks the common 09/27 case.
10. **New assets work in editor but disappear from the executable.** Required
    sprites use constructor-time references; the clean package inventory and
    packaged journey must exercise them.
11. **A broad commandlet run rewrites unrelated content.** The new import flag
    filters the source list before import and skips UI sounds and Widget
    Blueprint generation.
12. **The surface becomes too dark or visually noisy.** Review at overview and
    construction zooms checks Riverbend palette fit, marking contrast, worker
    readability, and at least 60 percent world prominence against VA-02.

## Required proof

Retain only candidates that pass seam, scale, palette, provenance, and
recognizability review. Compile the editor before import, import only the two
new surfaces, recompile editor and game targets, run focused and complete
automation, inspect delivery/mid-build/final captures, build a clean Development
package with before/after cache measurement, confirm both new sprite assets in
the IoStore inventory, and run the packaged Phase 1 journey.
