# Phase 0 Verification Record

**Status:** Passed
**Date:** 2026-07-25 local / 2026-07-26 UTC
**Scope:** PI-01 Phase 0, TS-01 through TS-09 and CT-01
**Engine:** Unreal Engine 5.8.0, Win64

## Outcome

The empty-airport foundation compiles, runs deterministically, saves and restores, renders an orthographic 2D proof screen, packages in Development and Shipping, and operates with inbound and outbound network access denied. The Shipping package contains no AMSim editor/test modules or prohibited editor-automation runtime.

No aircraft source asset was copied into the project.

## Measurement host

This is the Phase 0 verification host, not a minimum or release-reference specification:

- CORSAIR VENGEANCE i5200;
- Windows 11 Pro 10.0.26200, build 26200;
- Intel Core Ultra 9 285K, 24 physical/logical cores;
- 68,024,098,816 bytes RAM, approximately 63.4 GiB;
- NVIDIA GeForce RTX 5090, driver `32.0.16.1074`.

The release/reference performance tier must still be selected before Phase 1 closes.

## Automated contract evidence

`Automation RunTests AMSim.Phase0` discovered and passed all 14 tests:

1. `AMSim.Phase0.Content.DefinitionValidation`
2. `AMSim.Phase0.Content.ImportManifestTemplate`
3. `AMSim.Phase0.Performance.LogicalScaleFixture`
4. `AMSim.Phase0.Presentation.OrthographicCamera`
5. `AMSim.Phase0.Presentation.ProxyPoolReuse`
6. `AMSim.Phase0.Save.CoalescingAndCorruption`
7. `AMSim.Phase0.Save.RoundTripAndBackup`
8. `AMSim.Phase0.Simulation.FixedClock`
9. `AMSim.Phase0.Simulation.NamedRandomStream`
10. `AMSim.Phase0.Simulation.ReplayChecksum`
11. `AMSim.Phase0.Simulation.SnapshotContinuation`
12. `AMSim.Phase0.Spatial.BaseOccupancy`
13. `AMSim.Phase0.Spatial.ChunkAddress`
14. `AMSim.Phase0.Spatial.ReservationOrder`

The final logical-scale fixture recorded `0.347 ms` per simulation step, `0.001 ms` per snapshot, and checksum `11684030530412666515`.

## Package and proof-scene evidence

The final pipeline command was:

```powershell
.\scripts\phase0\Invoke-Phase0Pipeline.ps1 `
  -PackageRoot 'D:\AMSimPhase0Final-20260726' `
  -SkipFirewall
```

The pipeline generated:

- result: `AMSim/Saved/Phase0/pipeline-result.json`;
- Development executable: `D:\AMSimPhase0Final-20260726\Development\Windows\AMSim.exe`;
- Shipping executable: `D:\AMSimPhase0Final-20260726\Shipping\Windows\AMSim.exe`;
- proof screen: `D:\AMSimPhase0Final-20260726\Development\Windows\AMSim\Saved\Phase0\proof-screen.png`;
- packaged notice inventory: `Windows\NOTICES.txt`.

The 1920 x 1080 proof ran for `5.000017 seconds` and sampled 5,823 frames:

- average: `1164.596 FPS`;
- p99 frame time: `1.488 ms`;
- maximum sampled frame time: `72.424 ms`;
- save/load continuation: passed;
- deterministic continuation checksum: `4404817232840225737`;
- screenshot non-black sample ratio: `1.0`;
- proof-screen SHA-256: `0768963C38B997D92CAAD17BC287AD58E455DDDE5ECC09109AE35A3CEA480F3C`.

The Shipping package contained 27 files and 306,967,407 bytes (292.75 MiB). Its executable SHA-256 was `DEF5538AC925382C0F5FFE98FE9689C66EA921595043D72D6BE9A2C321DE474B`.

Shipping scans found zero:

- forbidden loose files;
- forbidden staged-manifest text matches;
- forbidden build-receipt matches;
- TCP socket observations during the clean five-second launch.

The forbidden set covered `AMSimEditor`, `AMSimTests`, `ModelContextProtocol`, `PythonScriptPlugin`, `RemoteControl`, and `ToolsetRegistry`.

## Network-denied evidence

The elevated network-denied verification used temporary exact-executable Windows Firewall rules for inbound and outbound traffic on all profiles:

```powershell
.\scripts\phase0\Test-Phase0NetworkDenied.ps1 `
  -PackageRoot 'D:\AMSimPhase0Final-20260726'
```

`AMSim/Saved/Phase0/network-denied-result.json` records:

- Development exit code `0`;
- Development save/load continuity passed;
- Development average `1221.316 FPS`, p99 `1.237 ms`;
- Development exposed only the expected local developer trace-control listener while firewall rules were active;
- Shipping launched cleanly for five seconds with zero TCP sockets;
- overall result passed.

The script removed its firewall rules in cleanup. No matching firewall rule or AMSim process remained afterward.

## Content and provenance boundary

The CT-01 import-manifest template contains 21 columns and zero asset rows. Its validator passes. `AMSim/Content` contains no PNG, SVG, or aircraft asset copied from `C:\Users\dave\Documents\Joes_Game\dist\assets`.

## Phase gate result

| Gate | Result |
| --- | --- |
| Identical empty-world replay checksum/events | Passed |
| Save/load continuation | Passed |
| 60 FPS 2D proof scene | Passed |
| Clean Development and Shipping packages | Passed |
| Editor/MCP/test code absent from Shipping | Passed |
| Network-denied Windows smoke test | Passed |
| CT-01 template without aircraft import | Passed |

Phase 0 is complete. The next production phase is Phase 1, the grass-airfield vertical slice.
