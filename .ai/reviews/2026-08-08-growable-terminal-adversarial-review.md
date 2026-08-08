# Growable terminal starter vertical slice — preliminary adversarial review

Date: 2026-08-08

## Failure cases and controls

- **Two unrelated terminal systems survive:** schemas 1-9 map the existing
  passenger fixture into the same spatial records and pooled renderer used by
  the GA terminal; the old fixed-coordinate renderer is retired after parity.
- **Immediate spending makes mistakes punitive:** the ghost shows price and
  validity before release; invalid input does nothing; queued or in-progress
  edits remain fully refundable through Undo.
- **Undo becomes an economy exploit:** only unfinished edits remain in the undo
  history. Completed facilities require demolition and use balance-owned
  salvage rather than a full refund.
- **Remodeling traps people or bypasses security:** commands validate egress,
  accessibility, controlled boundaries, active gate paths, and local closure
  topology before charging or queuing work.
- **The roof disagrees with the interior:** roof and exterior footprint are
  derived exclusively from completed spatial cells. Active extensions retain
  construction treatment and do not receive a finished roof.
- **Cutaway mode feels like a separate screen:** selection and zoom drive a
  reversible roof fade in the airport world, preserve surrounding context, and
  restore the previous map camera on exit.
- **A furnished starter removes player agency:** the initial layout is compact,
  fully editable, and contains only GA-scale essentials; passenger-processing
  construction remains progression-gated.
- **Construction is cosmetic:** jobs use ordinary workers, travel time, local
  closures, stage transitions, and authoritative completion before the final
  object becomes usable.
- **Generated art ships without review:** target frames are design evidence;
  runtime candidates require contact-sheet approval, normalized footprints,
  provenance, alpha/seam checks, and cooker-visible references.
- **Ground-floor delivery falsely claims GS-08 completion:** PI-11 explicitly
  defers upper floors and vertical routing and does not mark that requirement
  delivered.
- **Large source files cross the project limit:** new behavior lands in focused
  modules; tests fail the existing organization audit if a touched file exceeds
  2,000 lines.
- **Save migration loses a live airport:** migration fixtures cover empty,
  starter, under-construction, open passenger, active-flight, baggage, and
  reconciliation boundaries with pre/post identity and checksum assertions.

## Preliminary verdict

No blocking architecture or product-safety issue. Implementation remains gated
on target-frame approval and must prove migration, route safety, ledger
reproduction, interaction recovery, and packaged asset presence before closeout.
