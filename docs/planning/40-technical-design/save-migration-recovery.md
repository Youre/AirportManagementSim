# TS-05: Save, Migration, Backup, and Recovery

**Status:** Approved baseline  
**Owner:** Persistence  
**Last updated:** 2026-07-25

## Outcome

Players can maintain independent airports confidently through asynchronous saves, schema evolution, backups, interruption, and recoverable corruption.

## Save set

Each slot is a directory containing:

- small metadata readable without loading the world;
- one authoritative snapshot;
- one previous-known-good backup;
- optional crash-recovery snapshot;
- preview image;
- bounded diagnostic record with no personal data.

Metadata includes slot ID, player label, airport name, map, created/played timestamps, game time, specialization summaries, screenshot reference, save schema, simulation rules version, content-manifest hash, and last result.

## Snapshot contract

The snapshot contains authoritative simulation state, stable IDs and next-ID counters, random-stream state, time/calendar/weather, all domain records, economy, unlocks, ratings, objectives/events, active construction/incidents, player policies, and required content IDs. It excludes Actors, Widgets, component pointers, transient caches, derived routes, audio clips, and presentation proxies.

Settings that apply across slots—audio, accessibility, controls—live in a separate local profile and never carry airport progression.

## Save operation

1. Request save.
2. At a fixed-step boundary, create an immutable snapshot and capture metadata.
3. Serialize/compress on a worker thread.
4. Write to a temporary path.
5. Verify header, size, checksum, and deserialize smoke check.
6. Rotate current to backup and atomically replace current.
7. Report success or preserve the prior valid save on failure.

Autosave occurs every 10 real minutes, after major construction/contract/incident resolution, and on safe exit. It never stalls simulation for prolonged disk work. Only one write per slot runs at once; a later request coalesces.

## Loading and migration

Loading validates file header, checksum, schema, required content, and invariant set before world activation. Migrations are ordered, one-way pure transforms from old schema to new; the original file remains untouched until migrated validation succeeds. Unsupported future versions are rejected with a clear message.

Missing optional content maps through documented aliases or a safe placeholder. Missing required map/rule content blocks load and preserves files. After activation, caches/routes rebuild and a deterministic checksum is recorded.

## Recovery

If current fails, offer previous-known-good, then crash recovery. Never silently overwrite a corrupt file. The UI reports which version/date is being restored. A manual "Create recovery copy" operation writes a separate slot ID.

## Acceptance and sources

Tests cover async round trip, interrupted write, corrupt checksum, backup rotation, coalescing, every migration fixture, missing content, future version, mid-operation state, independent slots, and deterministic continuation. Save compatibility fixtures are retained for every shipped schema.

Source: [Saving and Loading Your Game](https://dev.epicgames.com/documentation/unreal-engine/saving-and-loading-your-game-in-unreal-engine).

