# Construction Patrol and Movement Facing Adversarial Review

Date: 2026-08-02
Status: Approved with controls below

## Failure modes and controls

1. **Workers still appear stationary.** Use a 15-game-minute out-and-back
   patrol rather than the full two-hour surface-progress span; test two bounded
   construction times for different positions.
2. **Workers teleport when a loop restarts.** Use a triangular wave with equal
   positions at the cycle boundary and test start/end continuity.
3. **Workers all overlap.** Stagger phases and assign separate runway, taxiway,
   and road/gate work paths.
4. **A worker walks beyond the authored project.** Derive every path from the
   committed runway, taxiway graph, road, or fixed gate context and keep runway
   patrols inset from thresholds.
5. **Sprites face backward.** Account for the source art's negative-Y forward
   axis and test positive/negative X and Y directions independently.
6. **Stationary objects jitter or lose their last facing.** Only update facing
   when the intended movement vector is non-zero.
7. **Pause no longer freezes presentation.** Use authoritative game time only;
   no per-frame wall-clock animation or actor tick is introduced.
8. **8x makes workers visually incoherent.** The 15-game-minute cycle remains
   visible for roughly 1.9 real seconds at 8x and 15 seconds at 1x.
9. **Inspection becomes visually empty.** Keep the crew patrol active through
   inspection while hiding the delivery truck according to existing stage
   rules.
10. **Presentation mutates gameplay.** Component transforms remain write-only
    presentation outputs and have no simulation or save references.

## Required proof

Compile editor and game targets, run focused movement/presentation tests, run
the complete automation suite, capture the rendered dispatch and building
states, verify zero line-limit and diff-check violations, and record the result.
