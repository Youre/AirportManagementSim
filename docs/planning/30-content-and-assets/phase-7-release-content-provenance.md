# Phase 7 Release Content and Provenance

**Status:** Riverbend RC1 technical identity recorded; owner acceptance pending
**Owner:** Content and release engineering  
**Last updated:** 2026-08-02

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

## UI sound effects

The 28 short, nonverbal UI one-shots in `SourceAssets/Audio/UI` are approved
for UI-only runtime use. They were generated on 2026-08-02 with ElevenLabs
Sound Effects model `eleven_text_to_sound_v2` in MP3 44.1 kHz / 128 kbps
format. `ui-sfx-manifest.json` retains the exact prompts, requested durations,
provider/model, and SHA-256 checksum for every file; all 28 checksums matched
during integration review.

The source MP3 masters remain outside cooked content. The presentation asset
commandlet imports them to `/Game/Audio/UI` as short stereo, non-looping UI
sound waves and applies reviewed, non-destructive per-cue volume trims because
the generated source levels vary materially. Runtime selection is a typed
catalog with constructor-visible references; no online generation, endpoint,
credential, or runtime string asset load is included. UI meaning remains
redundant in text, captions, patterns, and color-independent state treatments.

## Release records

- `AMSim/Config/Phase7/Phase7ReleaseManifest.json` tracks candidate and gate
  identity.
- `AMSim/Config/Phase7/Phase7RightsInventory.json` links the retained
  provenance records and explicit exclusions.
- `AMSim/Config/Phase7/Phase7RadioPhraseCatalog.json` locks the 15 approved
  captions-first phrase families.
- `/Game/Phase7/Definitions` contains the cooker-visible release definitions.

Clean Development and Shipping packages succeeded from source commit
`24c17bc933c961d3e7119d70d4e4f32b010c50a6`. Both contain 628 cooked packages,
including all 15 Phase 7 definitions. The exact launcher/runtime hashes are
recorded in `Phase7ReleaseManifest.json` and the external
`AMSim/Saved/Phase7/release-attestation.json`.

The audio-enabled Development S01 journey and save/load pass. Shipping remains
open for eight seconds with zero TCP sockets. Human gates remain `pending`
until the owner performs them against those exact hashes.
