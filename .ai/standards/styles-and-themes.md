<!-- ai-project-init:begin managed -->
# Styles And Themes

Use this standard when the project has aesthetics, UI rules, visual design, themes, templates, or reusable UI components.

- Treat `.ai/styles/` as the source of truth for project aesthetics and UI template guidance.
- Check existing UI templates before creating new components.
- Use an applicable template when one exists.
- If no applicable template exists, create a new template first and then implement from it, even if it has only one current implementation.
- Keep UI components from echoing the user's implementation instructions as visible text.
- Placeholder text should be neutral content, such as `Test card`, not a literal restatement like `animated card that appeals to a 5 year old`.
<!-- ai-project-init:end managed -->

## Game-specific visual standard

- Preserve a 2D cartoon, primarily top-down presentation unless an approved concept decision changes it.
- Optimize aircraft silhouettes, runway markings, paths, and operational status cues for management-zoom legibility.
- Weather and day/night treatments must not obscure actionable simulation information.
- Do not reuse the visual interface of a reference game; extract interaction principles and create a project-specific visual language.
- Do not introduce 3D art, 3D mockups, or perspective-dependent UI into the asset pipeline.
