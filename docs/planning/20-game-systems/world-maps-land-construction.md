# GS-01: World, Maps, Land, and Construction

**Status:** Approved baseline  
**Owner:** World and construction  
**Last updated:** 2026-07-25

## Outcome and scope

Players turn a small serviced parcel into an airport by buying land and commissioning construction. This specification owns map boundaries, parcels, terrain restrictions, construction projects, deliveries, work zones, opening, closure, cancellation, and demolition. Placement interaction is owned by PX-04; connectivity is owned by GS-02.

## World and land

- Each save selects one curated fictional-region map. Maps define buildable bounds, starting parcel, purchasable parcels, roads, terrain masks, wind profile, climate, daylight, and authored landmarks.
- The initial map set must include temperate-flat, warm-dry, and cool-wet regions. They change constraints and appearance, not difficulty modes.
- A map may reserve scenery outside purchasable land. External political opposition, zoning hearings, and eminent-domain simulation are out of scope.
- Land is divided into named contiguous parcels. A parcel discloses price, area, terrain restrictions, existing access, and likely aviation value before purchase.
- Purchased land is permanent. Selling land is not supported, preventing broken infrastructure and save ambiguity.
- Terrain editing is limited to approved flattening and surface treatments. Mountains, water bodies, and external roads are authored constraints rather than freely sculpted terrain.

## Construction lifecycle

Every project follows `Proposed -> Funded -> Awaiting delivery -> Building -> Inspection -> Operational`, with optional `Paused`, `Cancelled`, or `Demolishing` branches.

1. A valid proposal shows footprint, required connections, cost breakdown, estimated labor, deliveries, and closures.
2. Confirmation reserves funds and space and creates a project.
3. Materials arrive from the map's external road connection in delivery vehicles.
4. Construction workers travel from an access point to the work zone and perform visible work.
5. Completed facilities pass automatic connectivity and safety inspection.
6. The player explicitly opens eligible facilities; a global or per-facility open/close control remains available.

For the starter project, confirmation immediately creates the visible graded
earthwork footprint and dispatches the truck and crew. A useful service road
shortens the same deterministic travel threshold used by simulation and
presentation. Finished runway, taxiway, and road surfaces replace their
earthwork treatment progressively during `Building`; markings wait for
`Inspection`. Pausing freezes the derived movement and reveal without losing
progress.

For the starter fixture, the authored thresholds are 30 game minutes for
delivery travel, 60 cumulative minutes before surface work, 180 cumulative
minutes before inspection, and 210 cumulative minutes before ready-to-open.
The existing useful-road rule reduces every threshold by 20 percent. These are
authoritative simulation thresholds; visual workers and surface reveal only
derive from them.

Routine construction cannot instantly appear. Small markings and movable objects may complete quickly, while buildings and paved networks require staged work. Exact durations come from CT-04 data and GS-20 defaults.

## Closures and continuity

- Only the facility, cells, network segment, or safety area affected by work closes.
- The preview must list every resulting closure and disconnected dependent before commitment.
- Nearby operations continue when a safe route and required separation remain.
- Work beside an active runway or taxiway requires a protected work boundary. If safe separation is unavailable, that movement area closes.
- Delivery vehicles and workers use service or construction access and must not cross active movement areas without an authorized route.

## Cancellation and demolition

- Before materials are delivered, cancellation returns all but the planning fee.
- After delivery or work starts, the refund equals unspent labor plus the recoverable-material fraction in GS-20.
- Demolition is itself a project. It creates a work zone, consumes time, may close adjacent facilities, and returns a small salvage value.
- Cancelling cannot erase delivered vehicles, strand workers, disconnect an occupied building, or bypass incident consequences.

## Required state and interfaces

Map definitions expose parcels, terrain cells, external connection ports, climate, and authored scenery. Project state records definition ID, footprint, owner facility, stage, reserved funds, delivered materials, work completed, dependencies, closures, and timestamps. Commands include propose, fund, pause, resume, cancel, demolish, open, and close. Events include project-stage-changed, delivery-arrived, facility-inspection-failed, facility-opened, and parcel-purchased.

## Visual reference

[VA-02 Build mode and runway extension](../30-content-and-assets/concept-art/02-build-mode-runway-extension.png) is the validation reference for construction proposal visibility, workers and deliveries, work-zone boundaries, and continued operation outside the affected closure. Its depicted cost and geometry are illustrative only.

## Edge cases and acceptance

- Insufficient funds, land ownership, slope, overlap, safety-area, or connectivity failures block confirmation with a specific remedy.
- A save loaded mid-project resumes the same stage, costs, workers, closures, and deliveries.
- If a project would remove the airport's last usable road access or every operational aircraft stand, confirmation requires a clear high-severity warning.
- Automated tests must prove parcel ownership, partial refund, affected-only closure, save/load continuity, and deterministic completion for identical commands.
