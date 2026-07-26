# AI-Assisted Visual Asset Generation

Generated images are candidate source assets or design evidence. They are never approved runtime content merely because a generation succeeds.

## Tool selection

- Use OpenAI Image Gen for concept art, UI mockups, composition exploration, coherent visual-language studies, and other high-fidelity or instruction-sensitive work.
- Use the local `comfyui-imagegen` skill for simple textures, surface variants, low-cost ideation, controlled image edits, background removal, and bulk candidates where its quality is sufficient.
- Prefer `qwen-edit` for conversational edits to an existing image, `qwen-2512-lora` for local Qwen text-to-image work, and BiRefNet/RMBG workflows for transparent cutouts as described by the installed skill.
- Do not silently switch generators when the requested or appropriate service is unavailable. Report the limitation and preserve the intended quality boundary.

## Project constraints

- Keep runtime art 2D, top-down where applicable, and consistent with the approved cartoon visual direction.
- Do not use generated imagery to introduce 3D runtime presentation, real operator branding, unreviewed aircraft identity, or unverifiable copied material.
- Concept art and UI mockups guide implementation but do not define gameplay numbers, exact geometry, or shippable text.
- Simple generated textures must still tile, scale, filter, and read correctly at the project’s management zooms.

## Provenance and review

For every retained candidate, record:

- generator/service and workflow or model;
- prompt and negative prompt when applicable;
- seed or request identity when available;
- source input paths for edits;
- generation dimensions and date;
- output checksum;
- post-processing and background-removal steps;
- intended use and review decision.

Before import, apply the owning content specification and manifest. Review silhouette, seams, alpha edges, palette, readability, accessibility, licensing risk, and visual consistency. Preserve source masters separately from derived Unreal assets.
