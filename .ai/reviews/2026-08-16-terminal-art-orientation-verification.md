# Terminal art orientation verification

Date: 2026-08-16
Result: Passed with continuing asset-polish debt

## Implemented correction

- Added one shared clockwise 90-degree presentation-basis correction to every
  schema-10 placed terminal object.
- Preserved saved and player-authored `QuarterTurns`; a Rotate action remains
  a relative clockwise quarter-turn.
- Left floors, walls, doors, routes, footprints, interaction ports, and source
  textures unchanged.
- Removed the Phase 1 presentation prerequisite from schema-10 terminal
  rendering so a mature save loaded directly from the new-airport screen can
  produce its roof and cutaway.

## Automated verification

- Focused `AMSim.TerminalGrowth.Presentation.RevisionGatingAndProxyReuse`
  automation passed after both corrections.
- Full automation passed 81 tests plus the retained warning-only Phase 0 save
  recovery probe, with zero failed, not-run, or in-process tests.
- The presentation regression covers direct Phase 3 restore without a prior
  Phase 1 snapshot, zero-turn art yaw at 90 degrees, and one-turn art yaw at
  180 degrees.
- Modified files remain below the 2,000-line standard; the largest touched
  source file is 1,339 lines.

## Packaging and release verification

- Clean Development and Shipping BuildCookRun archives succeeded under
  `D:\AMSim-TerminalOrientation-20260816-1`.
- The cook contains 748 packages.
- The Phase 7 release audit passed with zero runtime string asset loads, zero
  forbidden editor/MCP/test dependencies, zero required 3D candidates, and
  zero line-limit violations.
- The verified Development package was installed over the canonical test
  package without deleting save data. The save-slot set remained Phase1Smoke,
  Phase2Smoke, Phase3Smoke, Phase4Smoke, and VisualBaseline.
- The canonical inner executable SHA-256 is
  `D19DDCB04520AB0C08DD3BE1C6D2F2E61461A9F99662D4574CDC2F025851C3DC`.

## Packaged visual replay

The exact direct-load path was exercised in both the clean Development archive
and the refreshed canonical package:

`new-airport screen -> Load -> VisualBaseline -> Terminal`

The terminal rendered immediately rather than opening as an empty cutaway.
The equivalent-state comparison shows the dominant default seating rows
rotated clockwise from vertical to horizontal while intentionally rotated
objects retain distinct orientations.

Evidence:

- `AMSim/Saved/TerminalOrientationAudit/02-corrected-terminal-operations.jpg`
- `AMSim/Saved/TerminalOrientationAudit/03-terminal-orientation-before-after.png`
- `AMSim/Saved/TerminalOrientationAudit/04-canonical-corrected-terminal.jpg`

## Remaining intentional difference

The retained terminal object art is upright, but several source sprites still
contain faux perspective and are not strict nadir. That is separate asset
replacement debt and was not hidden with additional transforms.

## Cache check

No cache was cleared. After packaging, C retained 128.47 GiB free; project
caches measured 8.60 GiB and shared Unreal caches measured 0.81 GiB.
