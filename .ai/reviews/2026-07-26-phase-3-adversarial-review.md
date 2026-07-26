# Phase 3 Adversarial Review

**Status:** Passed for implementation; exit evidence mandatory
**Date:** 2026-07-26
**Scope:** PI-05 completeness, topology, reconciliation, save safety, visual fidelity, scale, and package isolation

## Review question

Could Phase 3 appear complete through a decorative terminal screenshot or scripted happy path while passengers, bags, security, accessible routing, landside access, persistence, scale, or normal player controls remain fake?

## Risks and countermeasures

### The terminal could be decorative

Every room, port, route, checkpoint, queue, and opening condition is authoritative state. Construction and connection commands precede opening; validation returns a stable cause and corrective action. The screenshot cannot count as behavioral evidence.

### Security could be a colored line

Every landside-to-sterile path must traverse a controlled checkpoint. A door edge cannot implicitly cross the boundary. Opening and restore validation both fail on bypass topology. Tests deliberately inject a bypass and close a checkpoint.

### Accessible routing could be a wheelchair icon only

At least one named passenger has a mobility profile requiring accessible links. The fixture records the chosen route and completes the journey through an accessible checkpoint. Removing that path blocks opening or creates an actionable wait; it never teleports the passenger.

### Passenger and bag totals could drift

Every bag retains an owning passenger and flight. Every step verifies population partitions and bag-state partitions. Boarding readiness checks boarded passengers against known checked bags. Arrival, reclaim, and regional exit totals must reconcile exactly.

### Named passengers could be UI-only

Names, parties, mobility, itinerary, bag count, needs, and journey state use save-stable records. The selected-party view maps from the immutable query/state and survives proxy replacement and save/load.

### Landside access could be summary text

Road, curb, parking, taxi, and bus records have capacity, route/open state, and passenger counts. Entry/exit reconciliation compares those records with terminal cohorts. Presentation vehicles are derived proxies only.

### The happy path could hide save corruption

Required save boundaries include funded/building terminal, partially connected topology, security queue, screened bags, boarding, arrival reclaim, and landside exit. Resumed continuation must match uninterrupted checksums and events. Restore rejects duplicate IDs, broken ownership, bypasses, and impossible state combinations.

### 10,000 logical passengers could be 10,000 sprites

The 10,000-logical simulation fixture and 2,000-visible presentation-pool fixture are separate assertions. Logical counts remain exact while proxy relevance caps visible objects. Neither can substitute for the other.

### The UI could technically function while ignoring VA-03

The gate uses an equivalent complete-terminal state at 1920 x 1080, 100% scale, then a 100-200% scale matrix. The reference and implementation are combined into a comparison board. P0-P2 differences in region proportions, world dominance, secure/access route legibility, party hierarchy, typography, palette, copy density, or component polish are blocking.

### Generated concept detail could leak in as unreviewed content

The concept image is reference evidence only. Runtime content is project-owned, fictional, reproducible 2D content with provenance. Incidental route codes, values, logos, equipment, and passenger totals are not copied.

### Editor tooling or runtime loads could leak into Shipping

Shipping inventory, receipts, text/files, socket observation, source scan, and IoStore inventory must show zero MCP, Python, editor, test, RemoteControl, Toolset, runtime string-load, missing Phase 3 asset, or required 3D match.

## Exit lock

This review passes implementation only. Phase 3 cannot close until the behavioral, save, scale, rendered comparison, packaged journey, direct asset inventory, offline, and dependency evidence described above exists.
