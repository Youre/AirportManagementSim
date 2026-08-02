# UI Audio Integration Adversarial Review

Date: 2026-08-02
Status: Approved with controls below

## Failure modes and controls

1. **The source count is assumed from conversation.** The manifest and disk
   are authoritative. Both currently contain 28 matching one-shots; integration
   must fail on a missing, extra, or checksum-mismatched retained source.
2. **A cue is effectively inaudible or startling.** Record measured levels and
   apply reviewed, non-destructive per-wave volume trim. Do not overwrite the
   MP3 masters.
3. **Hover sounds become exhausting.** Use the soft cue, suppress disabled
   controls through Slate behavior, and apply a bounded cooldown to explicit
   high-frequency cues.
4. **A generic click masks authoritative failure.** Result cues are selected
   after command evaluation. Accepted and rejected scheduling, load, build,
   offer, and recovery outcomes remain distinct.
5. **One click produces two prominent sounds.** Generic pressed feedback stays
   quiet; semantic result cues carry the stronger meaning. Do not attach two
   semantic cues to the same outcome.
6. **UI audio becomes the only status channel.** Existing label, border,
   pattern, focus, diagnostic, and caption behavior remains mandatory.
7. **Sound assets disappear from Shipping.** Typed constructor references and
   explicit `/Game/Audio/UI` cook coverage are both checked against IoStore.
8. **Runtime loads from source paths.** Runtime code may reference only cooked
   `USoundWave` objects. Source MP3 paths exist only in editor tooling and
   provenance records.
9. **Automation machines emit unwanted audio.** `-NoSound` prevents explicit
   playback, while tests inspect resources without producing sound.
10. **A failed import silently retains stale content.** The import commandlet
    replaces all 28 assets, verifies class/count/duration/channel properties,
    saves them, and returns nonzero on any mismatch.
11. **Future UI bypasses the audio language.** The style template requires the
    shared button style and typed semantic cue selection for every new
    interaction; label-derived or bespoke path loads are prohibited.
12. **Provider provenance is lost.** The Phase 7 provenance and rights inventory
    identify ElevenLabs, model, generation date, source manifest, and approved
    UI-only use without recording an API key.

## Required proof

Source/manifest audit, editor import, compile, focused audio tests, complete
automation, clean Shipping cook, direct IoStore inventory, forbidden-dependency
scan, offline launch, and an audible packaged replay must pass.
