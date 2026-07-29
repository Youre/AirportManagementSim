# Phase 7 Release Content and Provenance

**Status:** Implementation baseline; package hashes pending  
**Owner:** Content and release engineering  
**Last updated:** 2026-07-28

## Retained release content

Phase 7 retains the curated `Map.TemperateStarter` region and the approved
Phase 1-6 aircraft, fictional operators, facilities, world sprites, icons,
textures, and UI assets. It adds only project-authored data assets for
guidance, accessibility, offline radio coverage, rights, protocols, migration,
and release identity.

No new aircraft, external image, real operator brand, military core content,
3D asset, online service, or runtime string asset load enters through Phase 7.

## Music boundary

Files currently present under `SourceAssets/Audio/Music` are not automatically
release content. They remain outside cooked content and outside the Phase 7
commit unless a later review records generator/source, prompt or session,
rights, checksum, mix role, loop treatment, loudness, and explicit retention.

## Release records

- `AMSim/Config/Phase7/Phase7ReleaseManifest.json` tracks candidate and gate
  identity.
- `AMSim/Config/Phase7/Phase7RightsInventory.json` links the retained
  provenance records and explicit exclusions.
- `AMSim/Config/Phase7/Phase7RadioPhraseCatalog.json` locks the 15 approved
  captions-first phrase families.
- `/Game/Phase7/Definitions` contains the cooker-visible release definitions.

Package and executable hashes are filled by the release-candidate audit after
clean Development and Shipping packages succeed. Human gates remain `pending`
until the owner performs them against those exact hashes.
