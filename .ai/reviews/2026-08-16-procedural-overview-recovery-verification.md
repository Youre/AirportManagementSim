# Procedural overview recovery verification

Date: 2026-08-16
Result: Passed with continuing VA-01 polish debt

## Corrected failure

The reported screen was the retired fixed-coordinate Phase 3 terminal proof
renderer leaking into the root airport overview after a direct save restore.
It was not the schema-10 Terminal cutaway. Closing a destination disabled the
cutaway but did not explicitly restore mature-overview mode, and the presenter
defaulted to the legacy mode.

Direct restore, Back, and destination close now restore the exterior overview.
The legacy floors, rooms, routes, and props remain hidden at root level. The
Terminal action still opens the schema-10 cutaway, and Back restores the same
exterior presentation.

## Procedural presentation change

- Eight mature facility definitions now produce 24 deterministic procedural
  base, surface, and detail mesh components.
- The GA hangar, operations station, fuel area, parking lot, bus/taxi bay, rail
  platform, drop-off island, and perimeter gate use strict-nadir authored
  geometry instead of scale-stretched facility sprites.
- Borders, seams, entrances, parking and transit lanes, rail ties, fuel tanks,
  piping, and hazard marks carry identity without perspective distortion.
- Separate world heights and sort priorities prevent base/detail flicker and
  keep roads below facilities.
- One bounded apron-fixture sprite remains; small aircraft, people, vehicles,
  fixtures, and landscaping remain sprite-based deliberately.

## Automated and build evidence

- Focused `AMSim.TerminalGrowth.Presentation.RevisionGatingAndProxyReuse`:
  passed.
- Complete AMSim automation: 82 of 82 passed; zero errors; one retained
  warning-only backup-recovery probe.
- Clean Development BuildCookRun: passed; 750 cooked packages.
- Clean Shipping BuildCookRun: passed; 750 cooked packages.
- Shipping launcher remained responsive during the launch smoke.
- Phase 7 packaged release audit: passed with zero runtime string asset loads,
  forbidden editor/MCP/test dependencies, required 3D assets, or file-length
  violations.
- Cache status remained healthy: C free 128.36 GiB; project caches 8.6 GiB;
  shared Unreal caches 0.81 GiB. No cache was cleared.

## Packaged visual journey

1. Load `VisualBaseline`: healthy; the root presents one coherent exterior
   airport instead of room slabs and route-debug overlays.
2. Exterior overview: healthy; procedural movement surfaces and facility
   footprints share the same top-down world and layer order.
3. Open Terminal: healthy; the authoritative schema-10 furnished spatial
   cutaway opens without reviving the legacy proof renderer.
4. Back to overview: healthy; the coherent exterior returns and remains
   interactive.

Evidence is stored in `AMSim/Saved/ProceduralOverviewAudit/`:

- `01-user-broken-overview.png`
- `02-packaged-loaded-overview.png`
- `03-packaged-terminal-cutaway.png`
- `04-packaged-back-overview.png`
- `05-before-after-overview.png`
- `06-va01-procedural-overview.png`

## Visual comparison

The structural failure is corrected, but the result is not final VA-01 parity.
The three highest-impact remaining differences are:

1. VA-01 uses richer facility silhouettes, pavement variation, and landscape
   density; the runtime facility materials remain comparatively flat.
2. VA-01 shows denser apron and landside service activity; the runtime exterior
   is sparse between scheduled operations.
3. VA-01 includes a substantial bottom contextual inspector; the current root
   shell still uses only the compact radio/navigation strip.

These are bounded visual systems for later correction and do not justify
restoring the retired terminal proof renderer or mixed-perspective stretched
facility art.

## Canonical package

The verified Development package is installed at
`D:\AMSim-Current\Windows\AMSim.exe`. All 92 canonical Saved files were restored
byte-for-byte. The canonical inner executable SHA-256 is
`C9681F2FD307C5ABC48EBB63049CF17F36DE1ED07B117A9F616548AD7B1E5D5A`.
