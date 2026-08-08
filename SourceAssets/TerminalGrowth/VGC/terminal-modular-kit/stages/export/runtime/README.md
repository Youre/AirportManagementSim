# VGC Runtime Bundle

This directory is a self-contained Vector Graphic Composer runtime export.

Files:
- `vgc.svg`: layered SVG asset with preserved element and layer IDs.
- `graph.json`: interaction graph primitives, influence regions, and traversal hints.
- `runtime_metadata.json`: layer, assignment, graph summary, provenance, and warning metadata.
- `manifest.json`: asset index with hashes and bundle metadata.

The bundle is generic and does not assume a specific web, game, or app runtime.
