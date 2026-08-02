# Riverbend UI sound inventory

This is the reviewed Phase 7 UI-only sound inventory for AirportManagementSim.
It contains 28 generated source masters. Unreal imports them as non-looping
`USoundWave` assets under `/Game/Audio/UI`; the source MP3 files remain outside
cooked content.

## Interaction coverage

| Feedback family | Covered interactions | Planned assets |
|---|---|---|
| Navigation and focus | hover, keyboard/gamepad focus, primary/secondary click, tab switch, panel open/close, back | `ui_hover_soft`, `ui_focus_tick`, `ui_click_primary`, `ui_click_secondary`, `ui_tab_switch`, `ui_panel_open`, `ui_panel_close`, `ui_back` |
| Construction | arm a build tool, place a facility/path, magnetic snap or legal crossing, confirm, cancel/reset/undo, invalid geometry | `ui_build_start`, `ui_build_place`, `ui_build_snap`, `ui_build_confirm`, `ui_build_cancel`, `ui_invalid_geometry` |
| Offers and operations | offer available, accept, decline/reject, schedule accepted/rejected, ordinary notification | `ui_offer_available`, `ui_offer_accept`, `ui_offer_decline`, `ui_schedule_confirm`, `ui_schedule_rejected`, `ui_notification` |
| Persistence | save success, load success, load/recovery failure | `ui_save_success`, `ui_load_success`, `ui_load_failure` |
| Progression and risk | warning/attention, objective complete, capability unlock, incident alert, recovery success | `ui_warning_attention`, `ui_objective_complete`, `ui_capability_unlock`, `ui_incident_alert`, `ui_recovery_success` |

## Runtime integration

- `UAMSimUISoundSubsystem` owns the complete typed cue catalog, cooker-visible
  references, cooldowns, and `-NoSound` behavior.
- `AMSim::UITheme::ButtonStyle` supplies the shared hover and press sounds, so
  every current and future Riverbend button receives baseline audio without
  screen-specific wiring.
- Semantic actions play their result cue only after authoritative success or
  failure. Construction snapping, offers, schedules, persistence, objectives,
  capability changes, incidents, and recovery use the dedicated cues above.
- Import applies reviewed per-cue volume trims without modifying the source
  masters. The trims compensate for substantial differences in generated
  loudness and remain editable in the presentation asset commandlet.
- Reimport only this catalog with
  `-run=AMSimPresentationAsset -UISoundsOnly`; the scoped mode does not resave
  unrelated presentation textures, sprites, or Widget Blueprints.

## Design constraints

- Short one-shots, no voice, no lyrics, and no sustained ambience.
- Warm, rounded, lightly analog-digital timbre consistent with the Riverbend
  navy/cyan/amber UI language; critical feedback is urgent without sounding
  punitive or catastrophic.
- Every state remains understandable without audio through existing text,
  pattern, border, and caption treatments. These sounds are reinforcement, not
  the sole status channel.
- Keep source masters outside cooked content. Runtime assets must remain short,
  non-looping, assigned to the UI sound group, and present in Shipping.

## Provenance

The files use ElevenLabs Sound Effects (`eleven_text_to_sound_v2`)
through `POST /v1/sound-generation`. Prompts, durations, model, output format,
generation date, and checksums are tracked in `ui-sfx-manifest.json`. All 28
files were present and checksum-valid at runtime-integration review. The API key
is read from the local `ELEVEN_LABS_API_KEY` environment variable and is never
recorded in the repo.
