<!-- ai-project-init:begin managed -->
# Feature Lifecycle

Use this workflow for non-trivial implementation work.

1. Define the user-visible outcome and explicit non-goals.
2. Inspect affected architecture, contracts, state, and test surfaces.
3. Implement the smallest coherent change.
4. Add or update focused tests.
5. Run adversarial critique against regressions, security, migration, and operability.
6. Add observability hooks when behavior changes need runtime visibility.
7. Record migration implications and rollback notes.
8. Update `.ai/context/current-state.md` and related dynamic context.
<!-- ai-project-init:end managed -->
