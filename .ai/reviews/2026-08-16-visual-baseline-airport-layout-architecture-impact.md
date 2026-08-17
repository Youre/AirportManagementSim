# Visual-baseline airport layout architecture impact

Date: 2026-08-16
Status: Approved for implementation

## User-visible outcome

The `VisualBaseline` airport becomes a deliberately zoned review fixture:

- runway and parallel taxiway at the north edge;
- apron and gate pads in one uninterrupted airside band;
- the roofless terminal immediately landside of the gates;
- drop-off, access road, parking, transit, and rail outside the terminal;
- hangar, operations, and fuel facilities flanking the apron; and
- aircraft, vehicles, people, bags, and landscaping located in the zone whose
  activity they represent.

The grown terminal keeps its approved footprint, scale, furniture, and single-
canvas behavior. The correction creates space around it instead of shrinking
or hiding it.

## Architecture finding

The schema-10 snapshot serializes the terminal layout and simulation state, but
the mature exterior airport coordinates are deterministic presentation-fixture
data in `AAMSimWorldPresenter`. Regenerating the save alone would therefore
reproduce the current overlaps. The coherent correction is to centralize the
mature fixture geometry in a focused presentation-layout contract, consume it
from the presenter, validate its clearances in automation, and then regenerate
the ordinary `VisualBaseline` save against that corrected package.

## Boundaries

- `AAMSimWorldPresenter` remains the sole airport-world renderer.
- No simulation command, economy value, flight, passenger, baggage, capability,
  tenant, or event behavior changes.
- No save schema or migration changes.
- No 3D assets, runtime string loads, widget-local world geometry, or second
  camera/view are introduced.
- Gate pads may deliberately bridge the terminal-to-apron gap. Other mature
  facility footprints and ambient entities must not intersect the terminal.
- Existing incident and response overlays move with the corrected runway and
  service zones so the Phase 4 proof remains spatially truthful.

## Compatibility and rollback

Schemas 1-10 remain compatible. Existing saves receive the corrected mature
presentation when loaded; their authoritative state and checksums are
unchanged. Rollback is limited to the presentation-layout constants and their
presenter consumers. The baseline installer continues to preserve unrelated
save slots and backs up the prior `VisualBaseline` during explicit refresh.

## Required proof

- focused layout automation proves terminal-to-apron and terminal-to-road
  clearances plus no unintended terminal/facility overlap;
- all AMSim automation remains green;
- clean Development packaging and the release audit pass;
- `VisualBaseline` is regenerated through the ordinary Phase 4 fixture and
  refreshed without changing unrelated saves;
- a 1920x1080, 100-percent packaged capture is compared with VA-01 and the
  overlapping pre-change capture; and
- the canonical owner package remains the only test executable.
