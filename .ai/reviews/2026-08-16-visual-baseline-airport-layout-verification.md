# Visual Baseline Airport Layout Verification

Date: 2026-08-16 EDT

## Outcome

The deterministic mature-airport review layout is cleanly zoned. Runway,
taxiway, apron, gate pads, the permanently roofless terminal, curb/access road,
parking, transit, and support facilities no longer occupy conflicting
footprints. Terminal and furnishing scale are unchanged.

## Implementation verification

- `FAMSimMatureAirportLayout` is the focused coordinate contract for mature
  infrastructure, facilities, incident overlays, selection targets, ambient
  identities, and the terminal presentation anchor.
- Automated layout assertions prove the terminal anchor, apron and road
  clearance bands, and non-intersection of every support-facility footprint.
- The initial management camera remains player-owned and single-canvas; its
  default offset now frames the operating campus without a facility mode.
- No simulation record, command, economy rule, save schema, route, terminal
  cell, or object scale changed.

## Verification performed

- UE 5.8 Editor build: passed.
- Focused terminal presentation test: passed.
- Complete `AMSim.*` automation: 81 successes plus one retained warning, zero
  failures.
- Clean Development package: passed, 750 cooked packages.
- Packaged deterministic Phase 4 journey: passed with checksum
  `635592312302300856`; save/load continuity and all state assertions passed.
- Source Phase 7 release audit: passed.
- Packaged 1920x1080 capture and VA-01 comparison: inspected; support facilities
  clear the terminal, and the airport reads in deliberate airside-to-landside
  bands.
- Canonical package refreshed at `D:\AMSim-Current\Windows\AMSim.exe`.
  `VisualBaseline` schema-10 snapshot SHA-256 is
  `0aac593daa10bf0a51040759ef3f9e36a96af9918047a924c22277381f15ce38`.
  Phase 1-4 smoke save slots were restored byte-for-byte after regeneration.
- Canonical launcher SHA-256 is
  `8e61ae1487664610f75bcc9a2439dcbd48541f8ceeaef88a913bf9ee5a2f0cd5`;
  inner executable SHA-256 is
  `6964778416aca737268cece1faa4383baa39905b6991034c067427d402efae85`.
- Cache audit remained healthy: C retained 124.71 GiB, D retained 668.17 GiB,
  project caches measured 8.85 GiB, shared Unreal caches measured 0.81 GiB,
  and no cache was cleared.

## Manual follow-up

Load `VisualBaseline` and confirm the new zoning at the owner's preferred
window size. Landscaping/activity density, runway prominence, and richer
support-facility detail remain bounded VA-01 polish debt; overlap remediation
does not depend on those later improvements.
