# First GA Visit Readability Adversarial Review

**Date:** 2026-08-02
**Status:** Preliminary review passed

## Risks and controls

1. **Longer timing could delay the promised first arrival.** Only movement and
   service durations change. The scheduled landing still occurs within three
   game minutes of confirmation.
2. **A visual route could cut across grass or ignore custom geometry.** Taxi
   movement is derived from the committed segment graph, not fixed world points.
   The adapter must prove positions along connected segments and reverse the
   same route for departure.
3. **A scheduled aircraft could remain as a misleading static world object.**
   Visibility begins at `Inbound` and ends after `Outbound`; `Scheduled`,
   `None`, and `Completed` are explicitly hidden.
4. **Presentation interpolation could authorize gameplay progress.** It cannot.
   Simulation state remains the only transition authority, and the adapter
   consumes immutable snapshots.
5. **Fast-forward could cause multiple phrase intents in one UI refresh.** The
   root processes every newly appended intent in order. The radio queues speech
   while captions remain immediate.
6. **Queued speech could replay old calls after load.** The UI cursor initializes
   to the existing phrase history and queues only intents appended afterward.
7. **An unavailable speech provider could stall the queue.** Silent/fallback
   mode treats each short cue as immediately available and never blocks
   captions or simulation.
8. **Existing later-phase fixtures could become prohibitively slow.** Headless
   helpers advance directly to named timeline thresholds; the fixed-step count
   remains bounded and is measured by the full suite.

## Required evidence

- exact state thresholds and autonomous completion;
- scheduled/off-map and active/on-map visibility;
- movement across approach, runway, taxi route, stand, reverse taxi, takeoff,
  and outbound legs;
- service progress over the named turnaround interval;
- FIFO speech behavior while a provider reports busy;
- save/load/deterministic regressions, full automation, and rendered inspection.
