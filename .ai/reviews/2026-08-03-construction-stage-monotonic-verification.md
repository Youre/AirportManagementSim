# Construction-stage monotonicity — verification

Date: 2026-08-03

## Defect

After entering `Inspection`, the next construction update still satisfied the
older building threshold. Because the condition only checked “not Building,”
the state moved backward to `Building`; the following update moved forward to
`Inspection` again. The compact card therefore flickered between `SAFETY
INSPECTION` and `SURFACE WORK`.

## Correction

Building and inspection thresholds now apply only when the current enum stage
is earlier than the target stage. Construction progression is monotonic without
changing durations, save schema, balance, or presentation copy.

## Evidence

- UE 5.8 Editor target compiled.
- Focused `AMSim.Phase1.Construction.StarterAirfield` passed 1/1. Its regression
  advances to the road-adjusted inspection boundary, steps again, and proves
  the project remains in Inspection.
- Complete automation passed 75 tests plus the retained warning-only recovery
  probe; zero failed, not-run, or in-process tests.
- Development BuildCookRun passed and refreshed
  `D:\AMSimPlayable-20260803-FirstVisit\Windows\AMSim.exe`.
- Final game binary: 227,756,544 bytes; SHA-256
  `51E5597CBB195E6C7E2C16CF00D828ADB1B26344BE7BE71BD312D1A7CF79B8D8`.
