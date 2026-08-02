# Owner Smoke Correction Architecture Impact

**Status:** approved for implementation
**Scope:** Riverbend RC1 owner-smoke blockers
**Evidence:** `AMSim/Saved/Phase7/OwnerAudit-20260729/01-after-configuration-acceptance.png`

## User-visible outcome

- Packaged players can switch between borderless and a practical windowed mode.
- The starter-airfield proposal can be repositioned before funding.
- Accepted proposal geometry is the geometry rendered during construction and operation.
- WASD and middle-mouse drag pan the orthographic world; the wheel continues to zoom.
- Funding begins construction at 1x and makes the active time state explicit.
- Airport, Terminal, Regional, Advanced, and Major are persistent destinations.
  Locked destinations remain visible with a reason instead of disappearing.
- Save/load exposes selectable slots and their metadata rather than operating on
  a hidden creation-time text field.

## Architectural impact

- Phase 1 remains authoritative for proposal validation, persistence, economy,
  construction, and progression. Presentation may edit a local proposal, but
  only `CommitStarterPlan` mutates simulation state.
- World geometry derives from the saved `FConstructionProjectRecord::Proposal`.
  No presented position is fed back into simulation after commit.
- Camera movement remains presentation-only in `AAMSimCameraPawn`.
- Display and save-slot selection are local presentation/application settings;
  neither changes the save schema.
- Destination navigation remains a view concern. Eligibility controls whether
  a destination can open, not whether the destination is discoverable.
- New behavior is split into focused source files so the 2,000-line standard is
  preserved for the existing root and world-presenter modules.

## Migration and packaging

- Save schema 8 is unchanged because proposal geometry is already serialized.
- Existing saves render their stored proposal. Saves without a committed
  project continue to show no facility geometry.
- Window preferences use Unreal `UGameUserSettings`.
- No runtime string asset load, editor dependency, online dependency, 3D asset,
  or new external content is introduced.

## Rollback

- Window controls, save-slot presentation, and mouse-drag handlers can be
  removed independently.
- Authoritative world placement can fall back to the default geometry only for
  legacy invalid data, while the existing restore validator remains the
  primary protection.
