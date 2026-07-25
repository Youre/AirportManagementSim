# CT-03: Radio, Audio, and Offline Speech

**Status:** Approved baseline  
**Owner:** Audio and speech  
**Last updated:** 2026-07-25

## Outcome and scope

The airport sounds alive through frequent authentic radio, environmental audio, and fully captioned operational calls. Speech remains offline and replaceable. This specification owns phrase events, phrase rendering, speech-provider abstraction, caching, captions, mixing, and silent fallback.

## Phrase system

Simulation events produce structured phrase intents, never final synthesized audio. An intent contains phrase ID, speaker role, call sign, runway/taxi/facility tokens, urgency, parameters, game timestamp, and deduplication key. CT-04 owns localized templates and pronunciation entries.

Initial phrase families cover ATIS/weather summary, arrival contact, pattern entry, landing/takeoff clearance, hold short, runway crossing, taxi route, gate/stand assignment, go-around, service dispatch, boarding calls, passenger pages, weather warning, and emergency response.

Phrase construction uses authentic terminology with concise sentence structure. Optional help can explain a term after the call. Call signs, runway numbers, taxiway letters, and acronyms have explicit pronunciation tokens.

## Offline speech

A local `SpeechProvider` contract accepts normalized text, voice ID, rate, and cache key and returns playable audio or a failure result. Unreal's experimental TextToSpeech plugin is the first evaluated provider, isolated behind this contract. Generated clips are cached locally by phrase/template version, voice, parameters, and locale.

The game must remain fully functional if speech initialization or synthesis fails. Captions appear immediately, radio timing continues deterministically, and a short radio tone may replace speech. No phrase text, voice sample, telemetry, or player data is sent to a network service.

## Captions and mixing

Captions are on by default and include speaker label, exact operational meaning, and urgent visual treatment. They remain readable at accelerated time; low-priority calls may be summarized when the queue is crowded, but safety calls are never dropped.

Mix groups are radio, aircraft, ground vehicles, terminal ambience, weather, construction, incidents, UI, and music. Speed-up compresses ambient repetition without pitch-shifting critical speech into unintelligibility. Pause may finish the current safety phrase, then holds new calls.

## Acceptance

Tests must prove deterministic intent creation, token formatting, cache identity, deduplication, priority queueing, captions-first behavior, silent fallback, no network dependency, save/load of pending critical calls, and intelligibility at supported game speeds. Human review covers authentic terminology and child comprehension.

