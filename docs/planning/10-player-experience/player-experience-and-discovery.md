# Player Experience and Discovery

**ID:** PX-01  
**Status:** Approved  
**Depends on:** GS-15, GS-16, GS-17, TS-02  
**Integration phase:** Phase 1 foundation, expanded through all later phases

## Outcome

A child who understands basic written English and loves aviation can start a new airport, discover systems through direct interaction, recover from mistakes, and decide what kind of airport to build without completing a mandatory tutorial campaign.

## Experience contract

The game provides one forgiving intended balance. It does not expose difficulty presets, adaptive hidden difficulty, or a forced tutorial academy.

A new save follows this experience:

1. Select a curated fictional region.
2. Name the airport and choose a save slot.
3. Enter a paused map with a road entrance, owned starter parcel, delivery truck, worker bus, starting rewards, and a short objective card.
4. Place a starter grass runway, taxi connection, aircraft parking position, access connection, and basic airport building.
5. Confirm construction and watch the first project complete.
6. Open the airport explicitly.
7. Receive the first compatible GA visit.
8. Observe landing, taxi, parking, service or passenger exchange, fee award, taxi, and departure.
9. Receive optional achievement suggestions, not a mandatory next mission.

The first playable loop is successful when the player can explain why the aircraft was compatible, where it went, what it needed, and what reward the airport earned.

## Guidance model

Guidance uses contextual, dismissible help:

- the first use of a tool shows a short anchored explanation;
- invalid actions explain the cause and the nearest corrective action;
- authentic terms remain authentic;
- key aviation terms may expose a one-paragraph definition and a simple diagram;
- help never blocks play after it has been acknowledged once;
- all dismissed help remains accessible through a Help panel;
- the game never assumes an external manual or internet connection.

No mechanic requires memorizing a definition before use. The player may attempt an action, see the preview or validation result, and learn through the consequence.

## Objectives and achievements

The objective surface has three levels:

- **Suggested next step:** one optional contextual card, selected from feasible actions.
- **Achievement:** a persistent save-local accomplishment with a clear condition.
- **Capability milestone:** an achievement or collection of achievements that grants an unlock.

Suggested actions never expire and carry no penalty when ignored. At most three are visible. The selection system must not recommend an action the current map, balance, or unlocked content makes impossible.

Achievement progress is visible before completion. Hidden achievements are not used for capability progression.

## Recovery experience

The airport has no unrecoverable bankruptcy or forced game-over state.

When the player cannot afford any meaningful action:

- basic compatible traffic continues if safe facilities remain;
- essential operating costs cannot make the balance decrease below zero;
- a recovery suggestion identifies one achievable source of credits;
- optional assistance may grant a small, explicitly labeled recovery delivery;
- assistance reduces only the associated achievement reward, never access to content;
- the save remains playable after incidents and failed projects.

Major accidents may report aircraft loss, injuries, or fatalities abstractly. The recovery experience focuses on emergency response, closures, investigation findings, repairs, and safer reopening. Human harm is never animated.

## Save-local identity

Every airport save independently stores:

- airport name and region;
- achievements and capability milestones;
- help items already seen;
- persistent aircraft visitors;
- incident history;
- player-selected specialization goals;
- all simulation and construction state.

No save grants gameplay advantages to another save.

## Initial tuning

- Starting guidance cards: 3
- Maximum active suggested objectives: 3
- Starter offer target after readiness: immediate while paused
- First-aircraft target after the player confirms its schedule: within 3 game minutes
- Recovery assistance trigger: no affordable unlocked revenue-producing action and projected income below essential needs for 10 game minutes
- Context-help text target: no more than 45 words per card
- Critical alert reading target: one sentence plus one action button

## Visual references

Use [VA-01 Airport overview and HUD](../30-content-and-assets/concept-art/01-airport-overview-hud.png) to validate that the living airport remains the focus while guidance stays peripheral. Use [VA-07 Progression and specializations](../30-content-and-assets/concept-art/07-progression-specializations.png) to validate three optional objectives, equal endgame prominence, and a capability journey without a formal ending. The written progression rules control all unlock values.

## Acceptance criteria

- A fresh save can reach its first completed aircraft visit without opening a separate tutorial mode.
- Every blocking validation encountered in the opening flow names both the problem and a corrective action.
- Ignoring all suggested objectives does not stop sandbox progression.
- A zero-credit airport with one safe runway and stand can recover without restarting.
- Starting a second save does not inherit unlocks, achievements, aircraft history, or money.
- Serious-incident reporting contains no graphic human depiction.
