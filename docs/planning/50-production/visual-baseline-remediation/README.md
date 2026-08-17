# Visual Baseline and HUD Remediation

This workstream provides a repeatable, partially completed airport for visual
and interaction testing, then uses it to correct one concept-art mismatch at a
time. The reference for the first pass is VA-01, the airport overview HUD.

## Canonical visual-review save

The packaged save slot is `VisualBaseline`. It is generated from the existing
deterministic Phase 4 fixture and includes a mature Riverbend Field with a
runway, taxiways, terminal, gates, roads, support facilities, aircraft,
services, economy state, and active operational evidence.

Install it without rebuilding the airport:

```powershell
.\scripts\ui\Install-VisualBaselineSave.ps1
```

The installer targets the one owner-facing package at
`D:\AMSim-Current\Windows\AMSim.exe`. It refuses to overwrite the slot unless
`-Refresh` is supplied, validates the generated fixture, preserves unrelated
saves, and records package/save checksums under
`AMSim/Saved/VisualBaseline/visual-baseline-install.json`.

Use the baseline as follows:

1. Start `D:\AMSim-Current\Windows\AMSim.exe`.
2. Choose **Load**, then **VisualBaseline**.
3. Choose **Back to Airport** from the regional view to inspect the complete
   exterior airport.
4. Reuse the same state, viewport, UI scale, and camera framing for every
   before/after capture in a bounded visual iteration.

Run the installer with `-Refresh` only when the fixture or presentation state
has materially changed. The current installed schema-10 snapshot SHA-256 is
`0aac593daa10bf0a51040759ef3f9e36a96af9918047a924c22277381f15ce38`.

## Clean mature-airport layout

The mature review airport now follows the VA-01 site hierarchy from north to
south: runway, parallel taxi lane, apron and gate pads, the permanently
roofless terminal, then curb, access road, parking, and transit. Hangar,
operations, fuel, rail, and perimeter facilities flank those bands instead of
occupying the terminal footprint. One shared `FAMSimMatureAirportLayout`
contract owns the deterministic coordinates used by ordinary presentation,
incident overlays, selection targets, and layout tests.

The terminal scale and interior-content scale are unchanged. Gates are the
only surfaces allowed to bridge the apron-to-terminal clearance; all support
facility footprints are tested against the terminal bounds. The default
management framing was shifted just enough to show the operating campus
without introducing a facility-specific camera mode.

Evidence:

- [packaged overview](./audit/clean-airport-layout-phase4-overview.png)
- [VA-01 comparison](./comparisons/clean-airport-layout-va01.png)
- [comparison review](./comparisons/clean-airport-layout-va01.md)

## First bounded remediation: persistent HUD shell

The first iteration corrected the shell rather than changing simulation or
world content:

- condensed the header into one line and grouped play/speed controls with time;
- replaced the permanent 320-unit diagnostics rail with compact right-side
  Alerts, Flights, and Projects tools whose labels expand toward the map;
- hid locked destinations and removed the disabled current-destination button;
- kept the existing operations detail available on deliberate request;
- removed unused hidden UMG runway, taxiway, stand, hut, and aircraft
  placeholders so the Paper2D presenter remains the only world renderer; and
- made supported historical save metadata discoverable so schema migration can
  occur after selection instead of hiding valid baseline saves.

Evidence:

- [before comparison](./comparisons/va01-before.png)
- [after comparison](./comparisons/va01-after.png)
- [audit](./audit/audit.md)
- [after review](./comparisons/va01-after.md)

## Procedural infrastructure baseline

The next world pass replaces scale-stretched runway, taxiway, road, apron, and
gate geometry with shared flat procedural surfaces. Preview, construction, and
completed states now reuse one dimensional builder and the existing world-layer
contract. The retained packaged comparison, corrections, and continuing
VA-01/VA-02 debt are recorded in the
[procedural infrastructure review](./procedural-infrastructure-review.md).

## Remaining VA-01 debt

This is not full visual parity. The next bounded system is the root
selected-aircraft inspector: identity, readiness, service progress, and
contextual actions belong in the bottom region. After that, the airport-world
composition needs a denser site-wide framing pass, and the right activity tools
need badges plus category-specific concise content.

The written gameplay, accessibility, strict top-down 2D, and state-truth
requirements continue to override incidental values or facilities shown in the
concept art.
