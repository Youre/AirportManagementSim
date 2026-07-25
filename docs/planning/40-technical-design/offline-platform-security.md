# TS-09: Offline Services, Platform, and Security

**Status:** Approved baseline  
**Owner:** Platform and security  
**Last updated:** 2026-07-25

## Outcome and scope

The initial game is a private, fully offline Windows desktop product. It has no gameplay backend, account, cloud dependency, analytics service, advertising, monetization service, or multiplayer surface.

## Platform baseline

- Initial target: 64-bit Windows desktop, mouse and keyboard.
- Unreal baseline: 5.8 project version, pinned per release branch.
- Saves, settings, speech cache, logs, and screenshots are local.
- The game must launch, create a save, play, save, load, and recover while outbound network access is denied.
- System clock is used only for local metadata; simulation outcomes use save time.

Controller, Steam/platform services, cloud saves, workshop/mod distribution, mobile, console, macOS, and Linux are future decisions, not hidden initial requirements.

## No-backend posture

There is no external database or API. Static definitions ship in cooked content. Dynamic state lives in the versioned SaveGame snapshot. SQLite is not required initially; adoption is permitted only if measured catalog/query needs justify it and save ownership remains explicit.

No external text-to-speech, AI generation, crash upload, telemetry, or rights lookup occurs at runtime. Development research and editor tools are outside the product and follow TS-07's localhost/editor boundary.

## Local data and child privacy

The game requests no real name, age, email, account, location, microphone, contacts, or payment information. Passenger/staff identities are fictional. Save labels are free text stored locally and must be length-limited and safely rendered.

Logs avoid generated personal-like names, full save content, and absolute user paths in player-facing exports. Local files are treated as untrusted input: validate schema, size, checksums, IDs, strings, and archive/path boundaries before use.

## Dependency policy

Use built-in Unreal 5.8 capabilities first. A third-party runtime plugin requires an architecture/security/license review, documented update ownership, offline behavior, deterministic impact, package test, and removal plan. Editor-only tools must not leak into Shipping dependencies.

## Acceptance

Tests must prove network-denied operation, independent local saves, safe corrupt-input rejection, bounded text fields, no runtime listener, no credential storage, no editor plugin in Shipping, and an inventory of packaged third-party notices. A release audit searches binaries/config/content for unexpected endpoints and secrets.

