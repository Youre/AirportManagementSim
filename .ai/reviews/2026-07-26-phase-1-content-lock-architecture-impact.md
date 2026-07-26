# Phase 1 Player-Facing Content Lock Architecture Impact

**Status:** Approved to implement
**Date:** 2026-07-26
**Scope:** PI-03 gate 7; CT-01/CT-02 aircraft, operator, art, and terminology

## Pre-change finding

The final audit found no explicit Phase 1 content-lock record. The internally
authored aircraft marker is rights-safe, but its symmetric cross does not expose
heading, its provisional `N-RB21` identifier resembles an invalid United States
registration, and several captions use `clear` where controller phraseology
requires `cleared`. Internal authorship alone does not satisfy PI-03 gate 7.

## Affected ownership

- `AMSimGameplay` retains the static fictional light-piston definition and adds
  review metadata only.
- `AMSimSimulation` retains all authoritative flight transitions and replaces
  provisional display/phrase strings plus the starter runway compatibility
  envelope. Stable aircraft/operator IDs, strong IDs, state ordering, rewards,
  schedule timing, and save schema remain unchanged.
- `AMSimUI` retains a derived, non-authoritative aircraft proxy. A directional
  project-authored vector silhouette and flight-state-to-heading adapter replace
  the symmetric cross.
- `AMSimEditor` enforces the machine-readable content-lock record during the
  project audit.

## Save and migration impact

No field or schema changes. New airframes use the reviewed fictional identifier;
existing schema-2 snapshots remain readable and retain their saved provisional
identifier. The stable content ID remains `Aircraft.LightPiston.Starter`, so no
content alias or save rewrite is required.

## Package and evidence impact

Runtime code, data assets, and cooked content change, so the previous four
package hashes become obsolete. Full automation, UI scale captures, fresh
Development/Shipping packages, package scans, elevated S15, package identity,
and the short reference harness must be regenerated before automated closeout
can return to green.

## Preserved invariants

- presentation cannot authorize simulation transitions;
- no source file is copied from the external aircraft candidate directory;
- no real aircraft or operator identity is claimed;
- the silhouette remains entirely 2D and color-independent in shape;
- all captions remain deterministic, immediate, local, and saved only as
  existing phrase-intent state.
