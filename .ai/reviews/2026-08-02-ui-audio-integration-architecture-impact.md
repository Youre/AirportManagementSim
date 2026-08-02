# UI Audio Integration Architecture Impact

Date: 2026-08-02
Status: Approved for implementation

## User-visible outcome

The reviewed Riverbend UI one-shots reinforce navigation, construction,
offers, scheduling, persistence, progression, warnings, incidents, and
recovery. Audio remains supplementary: every interaction and state continues
to expose its existing visual, textual, focus, pattern, or caption treatment.

## Asset boundary

- `SourceAssets/Audio/UI` remains the immutable source-master location.
- Unreal imports retained one-shots as `USoundWave` assets under
  `/Game/Audio/UI`; no runtime file decoding or string asset loading is used.
- The source manifest, exact SHA-256 checksums, generation provider/model,
  prompts, requested durations, measured duration/format, retention decision,
  and cooker inventory remain auditable.
- The importer sets every sound to non-looping UI behavior and retains short
  sounds in memory to avoid first-click latency.
- Source loudness varies materially. Per-asset non-destructive volume trim is
  stored on the imported `USoundWave`; source MP3 masters are not rewritten.

## Runtime boundary

- One `UGameInstanceSubsystem` owns the 28 typed semantic cue references,
  playback, cooldowns, command-line mute behavior, and validation.
- Native constructor-time hard references make every cue cooker-visible.
- The shared Riverbend `FButtonStyle` receives the generic hover and
  primary/secondary pressed sounds, so current and future themed buttons gain
  baseline feedback automatically.
- Context-specific cues are emitted at authoritative result boundaries rather
  than inferred from button labels. Examples include schedule accepted versus
  rejected, save success versus failure, and valid versus invalid placement.
- Simulation state, commands, checksums, save schema, and deterministic replay
  remain unchanged. UI audio never feeds back into simulation.

## Accessibility and coexistence

- `-NoSound` disables UI cue playback and preserves silent automation.
- Captions-first operational radio remains separate from short non-verbal UI
  feedback; UI one-shots do not receive redundant captions.
- Repeated hover/snap/notification cues use short cooldowns to avoid stacking.
- Missing cues fail validation and packaging audits but never prevent a player
  from using a control.

## Verification impact

Add cue-catalog, constructor-reference, style-resource, mute, cooldown, import,
cook, and packaged interaction coverage. Re-run complete automation, Shipping
dependency scans, and a short audible player journey covering navigation,
construction, schedule success/failure, save/load, and recovery.
