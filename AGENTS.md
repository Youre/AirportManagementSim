<!-- ai-project-init:begin managed -->
# Agent Instructions

Read `.ai/context/current-state.md` first. Treat `.ai/constitution/*` as stable doctrine.
Use `.ai/standards/linear.md` for Linear issue creation, status updates, comments, and issue context review.
Use `.ai/standards/code-organization.md` for file length and module organization rules.
Use `.ai/standards/styles-and-themes.md` and `.ai/styles/` when UI, themes, styles, or design templates are relevant.
Use `.ai/standards/secrets-and-security.md` for secrets, credentials, auth, and sensitive-data handling.
Use `.ai/workflows/verification.md` and `.ai/context/verification-log.yaml` to record verification.
Use `.ai/workflows/agent-handoff.md` and `.ai/handoffs/` when handing work between agents or worktrees.
Use `.ai/workflows/feature-lifecycle.md` for implementation work.
Before large changes, run required reviews: architecture-impact, adversarial-review.
After changes, update dynamic context and report verification.
Do not treat `.ai/scratch/` as source of truth.
<!-- ai-project-init:end managed -->

## Project-specific routing

For game planning, read `docs/planning/README.md` and the active documents it lists before adding or changing requirements.
Keep Unreal work 2D-first. Do not introduce 3D assets or copy assets from `C:\Users\Dave\Documents\AirplaneGame` without an approved import plan.
