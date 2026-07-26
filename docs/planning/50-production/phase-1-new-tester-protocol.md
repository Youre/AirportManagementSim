# Phase 1 Unassisted New-Tester Protocol

**Status:** Approved acceptance protocol; external session required
**Owner:** Player experience and quality engineering
**Last updated:** 2026-07-26

## Purpose

This protocol closes the human Phase 1 gate without turning developer knowledge
into hidden assistance. It tests whether a first-time player can complete the
starter grass-airfield journey, understand the operational cause-and-effect, and
use both audible local radio and the caption/fallback path.

## Tester and privacy

- Use someone who has not implemented, watched, or been coached through Phase 1.
- Prefer the target 5-10 audience with a parent or guardian present and consenting.
  An adult proxy may identify interaction problems, but the record must say so.
- Record only an anonymous session ID and broad tester band (`5-7`, `8-10`, or
  `adult proxy`). Do not record a name, voice, image, contact details, or save data.
- The facilitator may resolve a crash or hardware fault. Any gameplay hint makes
  the unassisted result fail and must be recorded.

## Preconditions

1. Use the final package hashes named by the Phase 1 verification record.
2. Start from an empty local save slot at 1920 x 1080 and 100% UI scale.
3. Confirm Windows output audio is audible before launching.
4. Keep this protocol hidden from the tester during the journey.
5. Capture start/end time and the number of gameplay hints, but do not capture
   personal audio or video.

## Unassisted journey

Give only this instruction:

> Create and name an airport, build and open the starter grass airfield, arrange
> its first visit, help the aircraft finish its visit, and save your airport.

The tester must independently:

1. name the airport and use an independent local slot;
2. recognize the starter plan, its cost, and any cause/remedy feedback;
3. commit the plan and understand visible construction progress;
4. open the completed airport;
5. recognize why the starter offer is compatible;
6. accept the offer, choose the exact slot, and assign Stand A1;
7. resume time and follow arrival, taxi, inspection, fueling, and departure;
8. identify the itemized reward and rating feedback;
9. save, change a visible state, load, and recognize the restored state.

The facilitator must not point, click, define terms, recommend a time speed, or
explain a status. Neutral prompts such as “What do you think that means?” are
allowed only after the tester has stopped acting and are recorded as observations,
not hints.

## Comprehension questions

After departure, ask without showing expected answers:

1. Why could this aircraft use your airport?
2. Where did it go after landing?
3. What did the aircraft need before leaving?
4. What did the airport earn, and why?

A correct answer may use the tester's own words, but must identify:

- the grass/light-aircraft/runway/stand compatibility;
- taxi to Stand A1;
- inspection and fuel;
- the flight-visit reward and its displayed explanation.

## Audio and fallback

During the main journey, confirm that at least one essential local radio call is
audible and that its caption communicates the same meaning.

Then launch the Development package once with local speech deliberately disabled:

```powershell
.\AMSim.exe -windowed -ForceRes -ResX=1920 -ResY=1080 -AMSimDisableLocalSpeech
```

Confirm that the screen announces caption mode, a local radio cue replaces speech,
captions remain immediate/readable, and the tester can explain one call from its
caption alone. This switch is compiled out of Shipping and cannot change
simulation state.

## Pass criteria

The human gate passes only when:

- the full journey and save/load check complete with zero gameplay hints;
- all four comprehension answers are correct;
- audible local speech and equivalent captions are confirmed;
- forced speech failure produces the local cue/caption path and remains
  understandable;
- no blocking accessibility or terminology issue is observed.

Any failed item keeps the gate open. Record the observation and owning follow-up;
do not reinterpret assistance as an unassisted pass.

## Acceptance record

Use the repository recorder after the session. It validates all four final package
hashes against
`scripts/phase1/Phase1AcceptanceManifest.json`, records only bounded
privacy-safe fields, and writes
`AMSim/Saved/Phase1/tester-acceptance-result.json`.

The facilitator sets a switch only after directly confirming that item. For a
child-band session, also set `ParentGuardianConsentConfirmed`. Replace the
example session ID and timestamps; do not put a tester name into the session ID.

```powershell
$acceptance = @{
    SessionId = 'session-001'
    TesterBand = 'adult proxy'
    SessionStarted = '2026-07-26T13:00:00-04:00'
    SessionEnded = '2026-07-26T13:25:00-04:00'
    GameplayHints = 0
    FirstTimeTesterConfirmed = $true
    EmptySaveSlotConfirmed = $true
    AudioOutputPrechecked = $true
    ProtocolHiddenFromTester = $true
    NoPersonalMediaCaptured = $true
    JourneyCompleted = $true
    SaveLoadRecognized = $true
    CompatibilityUnderstood = $true
    RouteStandUnderstood = $true
    ServicesUnderstood = $true
    RewardUnderstood = $true
    AudibleLocalSpeechConfirmed = $true
    EquivalentCaptionConfirmed = $true
    ForcedFallbackCueConfirmed = $true
    FallbackCaptionUnderstood = $true
    NoBlockingObservations = $true
    FacilitatorAttested = $true
    RecordFormalEvidence = $true
}
.\scripts\phase1\New-Phase1TesterAcceptanceRecord.ps1 @acceptance
```

If the session has a blocking observation, omit `NoBlockingObservations` and
provide bounded non-personal identifiers such as
`-BlockingObservationCodes UI-READABILITY -FollowUpReferences FOLLOWUP-01`.
Add `-AllowFailedEvidence` so the failed record is preserved without converting
it to a pass. A rehearsal must omit `RecordFormalEvidence`; even if every other
field is true, it remains non-certifying.

After both external records exist, run:

```powershell
.\scripts\phase1\Test-Phase1Acceptance.ps1
```

Phase 1 closes only when
`AMSim/Saved/Phase1/acceptance-result.json` reports `passed: true`.
