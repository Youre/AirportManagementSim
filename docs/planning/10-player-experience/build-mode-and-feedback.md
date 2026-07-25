# Build Mode and Operational Feedback

**ID:** PX-04  
**Status:** Approved  
**Depends on:** GS-01, GS-02, GS-15, TS-03  
**Integration phase:** Phase 1, expanded through Phase 6

## Outcome

Players can propose, validate, fund, construct, modify, and remove airport infrastructure while paused or running. Before committing, they understand cost, compatibility, closures, connections, and expected capability changes.

## Build workflow

Every build action follows:

1. Select a catalog item or network tool.
2. Preview placement on the grid.
3. See live validity, cost, required land, affected facilities, and missing prerequisites.
4. Adjust dimensions, orientation, variant, and endpoints.
5. Confirm a complete proposal.
6. Reserve credits and create a construction project.
7. Establish the affected work zone and closures.
8. Deliver materials and workers.
9. Complete stages.
10. Validate connections and activate the facility.

Network tools support click-drag-click paths with editable control points. Runways use centerline, heading, length, width, surface, and threshold options. Roads, taxiways, belts, and tracks use domain-specific curves and turn constraints.

## Preview states

- valid and affordable;
- valid but unaffordable;
- invalid geometry;
- outside owned land;
- blocked by active operation;
- missing prerequisite;
- disconnected;
- compatible with warning;
- demolition or closure required.

The preview provides both an overall result and per-segment diagnostics. “Invalid” alone is not sufficient.

## Planning and commit

Uncommitted proposals do not affect simulation, routing, capacity, or finances. The player may hold multiple proposals in a planning layer and commit them together.

Committed projects:

- receive stable project IDs;
- reserve the quoted credits immediately;
- use the validated geometry snapshot;
- can be paused before work starts;
- refund unspent work according to GS-15;
- cannot silently change cost or shape because content definitions changed.

## Closures

Only affected facilities close. A work-zone calculator identifies:

- runway safety area overlap;
- taxi or road segment obstruction;
- stand access obstruction;
- terminal room or corridor closure;
- belt, security, utility-capacity, station, or gate disconnection.

The confirmation dialog lists flights, paths, and services predicted to be affected during the first scheduled work window.

## Editing and demolition

Operational facilities are not edited in place. A modification creates a construction project with a before state and approved target state.

Demolition:

- requires confirmation for occupied or connected facilities;
- blocks if an entity cannot be safely rerouted;
- creates debris and worker stages for major structures;
- refunds a tunable salvage fraction;
- never removes persistent tenant or passenger state without an explicit relocation or cancellation outcome.

Undo applies only to uncommitted planning proposals. Committed projects use cancel and refund rules.

## Feedback

Build completion shows:

- capability added or removed;
- compatible new aircraft or services;
- remaining missing connections;
- any schedule now feasible or infeasible;
- rating or capacity change where applicable.

## Visual reference

Use [VA-02 Build mode and runway extension](../30-content-and-assets/concept-art/02-build-mode-runway-extension.png) to validate the distinction between existing and proposed geometry, spatial conflict marking, pattern-backed state, material/cost summary, and affected-only closure. PX-04 and GS-01 remain authoritative where the generated image compresses or combines validation states.

## Acceptance criteria

- Every catalog item provides live placement validity and total quoted cost before commitment.
- Committing an identical proposal in identical state produces identical project geometry and price.
- A runway upgrade closes only the affected runway and connected safety area.
- No active entity is deleted by construction or demolition.
- Network activation fails safely with a specific connection diagnostic.
- Save/load during every construction stage preserves project progress, reserved cost, closures, and workers.
