# Phase 4.5 Generated Content Provenance

**Status:** Candidate content kit under visual review  
**Owner:** Presentation engineering and visual design  
**Generated:** 2026-07-26  
**Generator:** OpenAI Image Gen  
**References:** VA-01, VA-03, VA-05, and VA-06

## Use and rights boundary

These files are project-generated fictional airport assets. They contain no
real operator identity, logo, readable brand, or copied game artwork. They are
approved for implementation evaluation, iteration, packaging tests, and this
project's eventual release subject to final visual approval.

The unmodified generated masters remain under
`SourceAssets/Phase4.5/Generated/`. Chroma-keyed and split derivatives are
reproducible build inputs. Only reviewed derivatives are imported into cooked
Unreal content.

## Airport operations atlas

The operations brief requested a strict top-down, flat 2D, premium-cartoon
five-by-four sprite sheet consistent with the approved Riverbend concepts. Its
twenty cells contained:

1. fuel truck, baggage tug, baggage-cart train, operations van, fire/rescue truck;
2. construction truck, shuttle bus, taxi, rental car, deicer;
3. ramp worker, construction worker, security officer, terminal agent, passenger family;
4. passenger, suitcase, safety cones, portable stairs, pushback tug.

The background was a uniform magenta chroma field. The prompt prohibited
logos, text, perspective, shadows that break the top-down read, military
content, and photorealism.

| File | Dimensions | SHA-256 |
| --- | --- | --- |
| `Generated/airport-operations-atlas-source.png` | 1536×1024 | `1C8C8E275C86302B5066D5CE51EC2DAE02C24CAA8E5D11F11BFF96E85481D602` |
| `Generated/airport-operations-atlas-alpha.png` | 1536×1024 | `A8B5D262991FFE985F4B84DD212353AE1E98603499F09729C0DA325370B7BF94` |

The alpha master uses sampled key color `#FA03F9`. It was split into twenty
named PNG cells under `Sprites/Operations/`; `split-manifest.json` records the
cell-to-file mapping.

Unreal destinations:

- `/Game/Phase45/Presentation/Textures/Operations/`
- `/Game/Phase45/Presentation/Sprites/Operations/`

## Terminal content atlas

The terminal brief requested a strict six-by-four grid matching VA-03's
cutaway language. Its twenty-four cells contained:

1. check-in desk, bag-drop desk, security scanner, queue barriers, gate podium, seating;
2. conveyor, sorting table, baggage cart, reclaim carousel, information desk, restroom;
3. entrance doors, secure door, partition wall, curb shelter, bus-stop shelter, parking kiosk;
4. terminal floor, secure floor, caution hatch, direction arrow, accessible route, protection zone.

The prompt prohibited text, logos, perspective presentation, photorealism,
real brands, and background scenery outside the chroma field.

| File | Dimensions | SHA-256 |
| --- | --- | --- |
| `Generated/terminal-content-atlas-source.png` | 1536×1024 | `FC6CFA238B2D1D456F145BB96883FF5824EC5E63703B5670753D9FD7D39212E4` |
| `Generated/terminal-content-atlas-alpha.png` | 1536×1024 | `C94DE5ABD4FA2CA0B5A935ABFE21A9B1B35D3F62355DE8D9888B2E28E354157F` |

The alpha master uses sampled key color `#FA03FA`. It was split into twenty-four
named PNG cells under `Sprites/Terminal/`; `split-manifest.json` records the
cell-to-file mapping.

Unreal destinations:

- `/Game/Phase45/Presentation/Textures/Terminal/`
- `/Game/Phase45/Presentation/Sprites/Terminal/`

## Shared UI icon and specialization atlas

The UI brief requested an exact four-by-four family with no text, logo, or
brand marks. The generated cells contain:

1. build, routes, timetable, staff;
2. overlays, alerts, flights, projects;
3. weather, services, passengers, baggage;
4. general aviation, flight school, charter, mixed-airport identities.

The first twelve cells are compact tool/status symbols. The last four are
larger pictographic specialization identities; baggage and passenger symbols
are reused for the future-locked cargo and passenger paths. The brief used the
accepted Riverbend navy/cyan/amber/coral/green language on a uniform magenta
chroma field and prohibited text, real brands, real operator identities,
perspective UI frames, and photorealism.

Original generator output:

`C:\Users\dave\.codex\generated_images\019f9c26-50dc-7221-8855-c36119b6bec3\call_myHfGYxzpU9EtCSBlMvSLe2h.png`

| File | Dimensions | SHA-256 |
| --- | --- | --- |
| `Generated/ui-icon-thumbnail-atlas-source.png` | 1254×1254 | `7AF8A562034FDA35122C7B3A8CA70F1F8B2324BC7027A5210F7BBA5C1DF77D4A` |
| `Generated/ui-icon-thumbnail-atlas-alpha.png` | 1254×1254 | `B855F833215A2FC5DFAB87C4B5278820D1F1C65CE09E6CAE9831BDD738129F90` |

The alpha master uses sampled key color `#E50BE8`, threshold `78`, and feather
`48`. The cleanup report recorded 992,164 transparent, 4,174 partial-alpha,
and 576,178 opaque pixels. It was split into sixteen named PNG cells under
`Sprites/UI/`; `split-manifest.json` records each crop, derivative checksum,
and the final alpha-master checksum.

Unreal destinations:

- `/Game/Phase45/Presentation/Textures/UI/`
- `/Game/Phase45/Presentation/Sprites/UI/`

Runtime use:

- `UAMSimOverviewView` holds hard references to five tool and four
  live-activity textures.
- `UAMSimProgressionView` holds hard references to six specialization
  identities, including the approved cargo/passenger symbol reuse.
- The generated identities remain schematic because VA-07 calls for detailed
  world-location thumbnails in the final quality pass.

## Mature airport site atlas

The mature-site brief requested an exact four-by-four, strict top-down atlas
that could supply the visible airport vocabulary required by VA-01 without
introducing 3D content. Its sixteen cells contain:

1. asphalt runway, asphalt taxiway, apron stand, access road;
2. regional terminal, GA hangars, operations/fire station, fuel farm;
3. parking lot, bus/taxi bay, rail platform, drop-off island;
4. tree cluster, landscape/rock/hedge cluster, perimeter gate, apron fixtures.

The prompt used the accepted Riverbend premium-cartoon shape language and
navy/cyan/amber/coral/green accents on a uniform magenta chroma field. It
prohibited text, logos, brands, perspective presentation, photorealism, and
background scenery outside the keyed field.

Original generator output:

`C:\Users\dave\.codex\generated_images\019f9c26-50dc-7221-8855-c36119b6bec3\call_9mMpnRE3I2TJPQuHnh2IGdif.png`

| File | Dimensions | SHA-256 |
| --- | --- | --- |
| `Generated/mature-airport-site-atlas-source.png` | 1254×1254 | `7213FCA0912960F5E8EB4C7D922E6469FAE846DC3153BDA7235793D1BCF044BC` |
| `Generated/mature-airport-site-atlas-alpha.png` | 1254×1254 | `63FF4F6299F0979428B963B00B346EDADE37149A170255ECCA341C74F09B1C41` |

The keyed-alpha master uses sampled key color `#E70ADA`, threshold `90`, and
feather `52`. The cleanup report records 778,775 transparent, 12,074
partial-alpha, and 781,667 opaque pixels. It was split into sixteen named PNG
cells under `Sprites/Site/`. The runway, taxiway, and access-road derivatives
are deterministically rotated 90 degrees clockwise so their authored
longitudinal markings align with the orthographic world composition.
`split-manifest.json` records every crop, rotation, derivative checksum, and
the alpha-master checksum.

Unreal destinations:

- `/Game/Phase45/Presentation/Textures/Site/`
- `/Game/Phase45/Presentation/Sprites/Site/`

Runtime use:

- `AAMSimWorldPresenter` holds serialized references to all sixteen sprites and
  owns the pooled site, taxi-connector, aircraft, vehicle, staff, passenger,
  and baggage proxies.
- `UAMSimRegionalOperationsView` activates the mature-site composition for the
  regional overview and active incident states. The terminal cutaway and
  capability view keep their distinct presentation modes.
- Flight, passenger, baggage, vehicle, and staff visibility remains derived
  from immutable simulation query state; the site kit never feeds positions or
  state back into simulation.
- VA-01 confirms that the required airport categories are now recognizable,
  but the kit remains schematic until spatial cohesion, ambient density,
  selection overlays, and contextual actions reach the concept-derived bar.

## Processing and review

- `scripts/phase45/SplitSpriteAtlas.py` performs deterministic alpha cleanup
  and grid extraction.
- `scripts/phase45/ChromaKeyAtlas.py` performs the recorded keyed-alpha
  cleanup before deterministic splitting.
- The source masters remain outside cooked content.
- `UAMSimPresentationAssetCommandlet` imports textures and creates PaperSprite
  assets.
- `AAMSimWorldPresenter` holds serialized hard references. Runtime string asset
  loading is not used.
- The current kit passed transparency, seam/cell-boundary, top-down
  readability, fictional identity, and editor asset-resolution checks.
- The current kit remains a candidate because VA-01, VA-03, VA-06, and VA-07
  comparison boards still identify density, architecture, world-overlay, and
  world-thumbnail quality gaps.
