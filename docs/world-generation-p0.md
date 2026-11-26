# World Generation P0 Supplement

This document records how the P0 supplement is implemented.

## Creation and first screen

- `WorldSeed.Parse` parses integer text; otherwise it uses the low 32 bits of a 64-bit FNV-1a hash as a signed seed.
- `CreateWorldScene` writes `manifest.bin` and `local_player.bin` immediately after construction, before any chunk exists.
- `World.GetSpawnPoint()` returns the fixed spawn `(0, surfaceHeight(0) + 3)` without clamping or searching.
- `ChunkStreamingSystem.HasCompletedInitialLoad` / `World.IsInitialLoadComplete` become true after the first load batch.
- The streaming marker is reset when switching dimensions.

## Data schema defaults

- `NoiseResource`: Perlin/None defaults, numeric noise/fractal/cellular enum values are accepted.
- `BiomeResource`: six climate targets default to 0.5; strictness defaults to 1.0.
- `DecoratorResource`: ore-oriented defaults (0.5 max height, 0.5/0.8 thresholds, 256 max count, depth-increase enabled) plus chest defaults (0.55 threshold, require surface). Type-specific seed/frequency defaults are applied by `ChunkGenerator`:
  - ore: seed offset 1024, frequency 0.01
  - loot chest: seed offset 4096, frequency 0.02
- `DecoratorResource` accepts `ore` and `chest_tile` aliases for `tile`.
- `StructureResource`: tree defaults follow the supplement (no leaves, 5..9 trunk, one leaf cluster, explicit layers required).
- `StructureResource` supports `data` + `trunk_data_index` / `leaves_data_index`.
- Numeric decorator/structure/condition type values are accepted.

## Boundaries and determinism

- `DataRepository.Register` rejects duplicate resource IDs and logs a warning; derived type lists are not polluted.
- Decorator traversal order is now explicit:
  - fill/ore/replace: X outer, Y inner
  - loot chest: Y outer, X inner
- Biome application order is deterministic first-occurrence order.
- Terrain, chunk and streaming state are owned by each `DimensionRuntime`, so caches and chunk tables are dimension-scoped.
- `ChunkStore.AcceptLoads` is disabled before unloading the old dimension and re-enabled only for the target, so late worker tasks cannot insert old-dimension chunks after a switch.
- Structure ordering is stable: priority descending, registration order for ties.
- Terrain results are weakly cached per dimension; climate values are computed on demand and are not retained in an unbounded cache.

## Tags and special tiles

- `TileResource.DeriveTags()` appends `replaceable`, `unbreakable`, `partial`, `solid`, `wall` and `decoration` according to the design.
- Tiles carrying `air` or `void` tags never derive `wall`/`decoration`.
- `solid` remains a data/tag semantic; collision is still configured by collision masks and boxes.

## Known deliberate differences

- An empty structure registry does not abort terrain generation; terrain still generates without structures.
- Missing tile resources resolve to no tile rather than a generated placeholder tile.
- Replacement source-terrain queries outside the current terrain block return no match instead of wrapping.
