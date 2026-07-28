# Phase 6 787-9 Scale and Livery Review

Status: **Approved**

Review board: [787-scale-livery-review-candidate.png](787-scale-livery-review-candidate.png)

## Proposed direction

- Fictional operator: **Riverbend Longreach**
- Recommended treatment: **River & Sun**
- Alternate treatment: **River Only**
- Strict top-down 2D silhouette with pixel-aligned outline treatment
- Ivory fuselage, navy tail and spine, cyan river line, amber sun/river mark
- Navy engines with cyan rings to preserve readability at gameplay scale
- Source normalization: **32 pixels per meter**
- Real-world reference dimensions: **62.8 m length, 60.1 m wingspan**

## Candidate source record

- Approved-use status: approved for the River & Sun production treatment
- External source master:
  `C:\Users\dave\Documents\Joes_Game\dist\assets\Boeing_787-9-B24vQXZX.png`
- Source dimensions: **719 × 776**, RGBA
- Source SHA-256:
  `6AA56CD2CDD7035E3EE3526CFF5A603751E03441DA11ECE6C7E6E550B9B04449`
- Source pixels copied into the project: **Yes, one approved reference copy
  under `SourceAssets/Phase6`; never cooked**
- Unreal runtime texture/sprite imported: **Yes, project-authored River & Sun
  master with 16 headings**
- Other external aircraft approved or imported by this review: **No**

The approved reference copy remains outside cooked content. Approval
authorized the project-authored high-resolution vector master, reviewed
heading variants, and corresponding Unreal Paper2D import. It does not
authorize other files from the external source directory.

## Review-board provenance

- Generated with OpenAI Image Gen for owner review on 2026-07-28
- Repository artifact SHA-256:
  `9E0DF73F25D8FEAEF168ABB21AD776462E6B60464C280658E04722AEF33DE713`
- This board is a direction-and-scale artifact, not a cooked runtime asset.

Known board-only typography artifact: the upper end of the right runway reads
`00R`. The authoritative reciprocal/parallel pairs are `09L/27R` and
`09R/27L`; the Figma, vector-master, runtime, and validation artifacts must use
the authoritative values. Phase 6 automation already verifies those pairs.

## Approval record

Select one:

- [x] Approve **River & Sun** (recommended)
- [ ] Approve **River Only**
- [ ] Request changes before asset production

Owner: Dave

Decision date: 2026-07-28

Notes: Approved in the project implementation task with the response
`river and sun`. Approval is limited to this recorded Boeing 787-9 source and
the fictional Riverbend Longreach treatment.
