# VGC Validation Review Checkpoints

- Status: `fail`
- Quality score: `90.424`
- Failed checks: `1`
- Next phase: `WEB-94`

## Checkpoints

### Visual Fidelity Review
- Status: `needs_review`
- Notes: Compare the cleaned SVG against the source image and confirm retained contour/point counts are sufficient for the requested fidelity profile.
- Artifacts: `C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\vectorize\vector\vector_manifest.json, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\vectorize\vector\raw.svg, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\vectorize\vector\run_manifest.json, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\cleanup\cleanup\cleanup_manifest.json, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\cleanup\cleanup\cleaned.svg, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\cleanup\cleanup\run_manifest.json`

### Runtime Bundle Review
- Status: `blocked`
- Notes: Confirm exported SVG/JSON bundle is complete and suitable for downstream application ingestion.
- Artifacts: `C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\export\runtime\export_package.json, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\export\runtime\vgc.svg, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\export\runtime_bundle.zip, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\export\runtime\run_manifest.json`

### Semantic Readability Review
- Status: `ready`
- Notes: Review layer labels, assignment coverage, and whether explicit layer hints are needed.
- Artifacts: `C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\segment\segmentation\segmentation_manifest.json, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\segment\segmentation\layered.svg, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\segment\segmentation\run_manifest.json`

### Interaction Mesh Review
- Status: `ready`
- Notes: Review node/edge topology and isolated nodes before runtime-specific integration.
- Artifacts: `C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\mesh\mesh\interaction_mesh.json, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\mesh\mesh\mesh_preview.svg, C:\Users\dave\Documents\AirportManagementSim\SourceAssets\Phase6\VGC\787-source-trace\stages\mesh\mesh\run_manifest.json`

## Refinement Actions

### action-01
- Type: `rerun_stage`
- Stage: `analyze`
- Reason: Visual fidelity is below the ready threshold; rerun the image workflow with `--fidelity-profile faithful` and inspect the contour budgets.
- Command: `skills/vector-graphic-composer/env/bin/python skills/vector-graphic-composer/scripts/vgc_orchestrate.py --source <source_image> --intent <intent> --entry-path image_tracing --fidelity-profile faithful --out <new_workflow_out>`
