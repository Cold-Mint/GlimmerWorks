using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Attachments;
using GlimmerWorks.Engine.World.Lighting;
using GlimmerWorks.Engine.World.Placement;
using GlimmerWorks.Engine.World.Save;
using GlimmerWorks.Engine.World.Storage;
using GlimmerWorks.Engine.World.Streaming;
using GlimmerWorks.Engine.World.Systems;
using GlimmerWorks.Engine.World.Tiles;

namespace GlimmerWorks.Engine.World;

/// <summary>
/// <para>WorldSettings</para>
/// </summary>
public sealed class WorldSettings
{
    public string Name { get; set; } = "New World";

    public long Seed { get; set; }

    /// <summary>
    /// <para>Directory containing manifest.bin, players and dimensions.</para>
    /// </summary>
    public string Directory { get; set; } = string.Empty;

    public bool AllowCheats { get; set; }

    public long InitialGlobalTick { get; set; }

    public int PreloadRadiusChunks { get; set; } = 4;

    public long CreateTime { get; set; }
}

/// <summary>
/// <para>IWorldChunkEntityPersistence</para>
/// <para>区块实体的可选持久化钩子。没有实体系统时保持为 null。</para>
/// </summary>
public interface IWorldChunkEntityPersistence
{
    /// <summary>
    /// <para>Copy entity-side container contents into tile states before saving a chunk.</para>
    /// <para>保存区块前把实体侧容器内容同步到瓦片状态。</para>
    /// </summary>
    void SyncContainersToTileStates(Chunk chunk);

    /// <summary>
    /// <para>Save entities inside the chunk and remove them from the entity manager.</para>
    /// <para>保存区块内实体，并从实体管理器移除。</para>
    /// </summary>
    void SaveAndRemoveChunkEntities(Chunk chunk);
}

/// <summary>
/// <para>World</para>
/// </summary>
public sealed class World
{
    private readonly Dictionary<string, DimensionRuntime> _dimensions = [];
    private double _growthAccumulator;
    private bool _saving;

    public World(
        WorldSettings settings,
        DataRepository dataRepository,
        ITileRegistry tiles,
        IEnumerable<DimensionRuntime> dimensions,
        DimensionRuntime currentDimension)
    {
        Settings = settings;
        DataRepository = dataRepository;
        Tiles = tiles;
        GlobalTick = settings.InitialGlobalTick;
        CurrentDimension = currentDimension;
        CreateTime = settings.CreateTime == 0
            ? DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
            : settings.CreateTime;
        GameVersionName = "GlimmerWorks";
        GameVersionNumber = (uint)EngineConstant.GameCode;
        Player = PlayerSaveState.CreateDefault(currentDimension.ResourceRef, settings.AllowCheats);

        foreach (var dimension in dimensions)
        {
            _dimensions[dimension.Resource.Id] = dimension;
            dimension.Store.World = this;
            dimension.Streaming.PreloadRadiusChunks = settings.PreloadRadiusChunks;
            dimension.Streaming.Scheduler.InvalidateCamera();
        }
    }

    public WorldSettings Settings { get; }

    public DataRepository DataRepository { get; }

    public ITileRegistry Tiles { get; }

    public long Seed => Settings.Seed;

    public long GlobalTick { get; private set; }

    public long CreateTime { get; private set; }

    public string GameVersionName { get; private set; }

    public uint GameVersionNumber { get; private set; }

    /// <summary>
    /// <para>Last allocated entity id. The next id is this value + 1.</para>
    /// </summary>
    public uint EntityIdIndex { get; private set; }

    public PlayerSaveState Player { get; private set; }

    public DimensionRuntime CurrentDimension { get; private set; }

    public IReadOnlyDictionary<string, DimensionRuntime> Dimensions => _dimensions;

    /// <summary>
    /// <para>Optional entity persistence hook used by the chunk store.</para>
    /// <para>区块存储使用的可选实体持久化钩子。</para>
    /// </summary>
    public IWorldChunkEntityPersistence? EntityPersistence { get; set; }

    public LightBuffer Light => CurrentDimension.Light;

    public bool IsInitialLoadComplete => CurrentDimension.Streaming.IsInitialLoadComplete;

    public GrowthSystem Growth => CurrentDimension.Growth;

    public ContainerSystem Containers => CurrentDimension.Containers;

    /// <summary>
    /// <para>Raised before the current dimension changes. The old dimension is still current.</para>
    /// </summary>
    public event Action<DimensionRuntime, DimensionRuntime>? DimensionChanging;

    /// <summary>
    /// <para>Raised after the current dimension changed.</para>
    /// </summary>
    public event Action<DimensionRuntime, DimensionRuntime>? DimensionChanged;

    /// <summary>
    /// <para>Raised when a tile drops an item. Persist or spawn it through the entity system.</para>
    /// </summary>
    public event Action<ResourceRef, int, TileVector2D>? ItemDropped;

    /// <summary>
    /// <para>Create a world from loaded data packs.</para>
    /// </summary>
    public static World? Create(
        AppContext appContext,
        WorldSettings settings,
        DimensionResource? startingDimension = null,
        Func<DimensionResource, IChunkStorage>? storageFactory = null)
    {
        var dimensions = appContext.DataRepository.GetResourceList<DimensionResource>().ToList();
        if (dimensions.Count == 0)
        {
            LogCat.Error("Cannot create a world: no dimension resources are registered.");
            return null;
        }

        foreach (var tile in appContext.DataRepository.GetResourceList<TileResource>())
        {
            if (!tile.Validate())
            {
                LogCat.Error($"Tile {tile.PackId}:{tile.ResourceId} has an invalid layer value {tile.Layer}. " +
                             "Layers must be 1, 2 or 4.");
            }
        }

        var tiles = new DataRepositoryTileRegistry(appContext.DataRepository);
        var chosen = startingDimension ??
                     dimensions.FirstOrDefault(static dimension => dimension.AllowAsStarting) ??
                     dimensions[0];

        var root = string.IsNullOrWhiteSpace(settings.Directory)
            ? SavePaths.GetSaveDirectory(
                Path.Combine(System.AppContext.BaseDirectory, "runtime"), settings.Name)
            : settings.Directory;
        settings.Directory = root;

        var runtimes = dimensions.Select(dimension => new DimensionRuntime(
            dimension,
            settings.Seed,
            appContext.DataRepository,
            tiles,
            storageFactory?.Invoke(dimension) ?? new MemoryPackChunkStorage(root))).ToList();
        var current = runtimes.First(runtime => runtime.Resource.Id == chosen.Id);

        return new World(settings, appContext.DataRepository, tiles, runtimes, current);
    }

    /// <summary>
    /// <para>Load a world manifest if present, otherwise create a new world.</para>
    /// </summary>
    public static World? LoadOrCreate(AppContext appContext, WorldSettings settings,
        Func<DimensionResource, IChunkStorage>? storageFactory = null)
    {
        if (string.IsNullOrWhiteSpace(settings.Directory))
        {
            settings.Directory = SavePaths.GetSaveDirectory(
                Path.Combine(System.AppContext.BaseDirectory, "runtime"), settings.Name);
        }

        var manifestPath = SavePaths.GetManifestPath(settings.Directory);
        if (!File.Exists(manifestPath))
        {
            return Create(appContext, settings, storageFactory: storageFactory);
        }

        try
        {
            var manifest = MemoryPack.MemoryPackSerializer.Deserialize<MapManifestMessage>(
                File.ReadAllBytes(manifestPath));
            if (manifest is null)
            {
                return Create(appContext, settings, storageFactory: storageFactory);
            }

            if (manifest.SaveFormatVersion > MapManifestMessage.CurrentSaveFormatVersion)
            {
                LogCat.Error($"Save format {manifest.SaveFormatVersion} is newer than supported " +
                             $"{MapManifestMessage.CurrentSaveFormatVersion}.");
                return null;
            }

            settings.Name = manifest.Name;
            settings.Seed = manifest.Seed;
            settings.InitialGlobalTick = (long)manifest.GlobalTickCount;

            var world = Create(appContext, settings, storageFactory: storageFactory);
            if (world is null)
            {
                return null;
            }

            world.CreateTime = manifest.CreateTime;
            world.GameVersionName = manifest.GameVersionName;
            world.GameVersionNumber = manifest.GameVersionNumber;
            world.EntityIdIndex = manifest.EntityIdIndex;

            var playerPath = SavePaths.GetLocalPlayerPath(settings.Directory);
            if (File.Exists(playerPath))
            {
                var playerMessage = MemoryPack.MemoryPackSerializer.Deserialize<PlayerMessage>(
                    File.ReadAllBytes(playerPath));
                if (playerMessage is not null)
                {
                    world.Player = PlayerSaveState.FromMessage(playerMessage);
                    settings.AllowCheats =
                        world.Player.PermissionLevel == PlayerPermissionLevel.Administrator;
                    var current = world.Player.GetCurrentDimension();
                    if (current is not null)
                    {
                        world.SwitchDimension(current, unloadOld: false);
                    }
                }
            }

            return world;
        }
        catch (Exception exception)
        {
            LogCat.Error($"Failed to load world manifest {manifestPath}: {exception.Message}");
            return Create(appContext, settings, storageFactory: storageFactory);
        }
    }

    /// <summary>
    /// <para>Advance world time and simulation.</para>
    /// </summary>
    public void Tick(long deltaMs)
    {
        GlobalTick++;
        CurrentDimension.AdvanceTime(deltaMs);

        _growthAccumulator += Math.Max(0, deltaMs) * WorldConstants.TicksPerSecond / 1000.0;
        var growthTicks = (long)_growthAccumulator;
        if (growthTicks > 0)
        {
            _growthAccumulator -= growthTicks;
            Growth.Tick(growthTicks);
        }
    }

    /// <summary>
    /// <para>Feed the current dimension's streaming system with a camera view.</para>
    /// </summary>
    public void SetCamera(CameraView camera)
    {
        CurrentDimension.Streaming.Tick(camera);
    }

    /// <summary>
    /// <para>Fixed new-player spawn: x = 0, y = surface height at x = 0 + 3. No search or clamping.</para>
    /// </summary>
    public TileVector2D GetSpawnPoint()
    {
        var surface = CurrentDimension.Terrain.Climate.GetSurfaceHeight(0);
        return new TileVector2D(0, surface + 3);
    }

    /// <summary>
    /// <para>Switch to another dimension. Old chunks are saved before the current dimension changes.</para>
    /// </summary>
    public bool SwitchDimension(ResourceRef dimensionRef, bool unloadOld = true)
    {
        var target = _dimensions.Values.FirstOrDefault(runtime =>
            runtime.Resource.PackId == dimensionRef.TargetPackId &&
            runtime.Resource.ResourceId == dimensionRef.ResourceId);
        return target is not null && SwitchDimension(target.Resource.Id, unloadOld);
    }

    /// <summary>
    /// <para>Switch to another dimension by resource id.</para>
    /// </summary>
    public bool SwitchDimension(string dimensionId, bool unloadOld = true)
    {
        if (!_dimensions.TryGetValue(dimensionId, out var target))
        {
            return false;
        }

        if (ReferenceEquals(target, CurrentDimension))
        {
            Player.Visit(target.ResourceRef);
            return true;
        }

        var previous = CurrentDimension;
        DimensionChanging?.Invoke(previous, target);
        previous.Streaming.Scheduler.ClearPending();
        previous.Store.AcceptLoads = false;
        if (unloadOld && !previous.Store.UnloadAll())
        {
            previous.Store.AcceptLoads = true;
            LogCat.Error("Dimension switch aborted because the old dimension could not be saved.");
            return false;
        }

        previous.Streaming.Scheduler.InvalidateCamera();
        previous.Streaming.ResetInitialLoad();
        CurrentDimension = target;
        target.Store.AcceptLoads = true;
        Player.Visit(target.ResourceRef);
        target.Streaming.Scheduler.InvalidateCamera();
        target.Streaming.ResetInitialLoad();
        target.UpdateAmbient(force: true);
        DimensionChanged?.Invoke(previous, target);
        return true;
    }

    /// <summary>
    /// <para>Refresh dirty lighting. Call once before rendering.</para>
    /// </summary>
    public void RefreshLighting()
    {
        foreach (var dimension in _dimensions.Values)
        {
            dimension.UpdateAmbient();
            dimension.Light.Refresh();
        }
    }

    /// <summary>
    /// <para>Place a multi-tile tile across chunks. Unloaded target chunks are skipped silently.</para>
    /// </summary>
    /// <summary>
    /// <para>Open a container and use the player's per-uuid loot snapshot when available.</para>
    /// </summary>
    public bool OpenContainer(Chunk chunk, int layerIndex, int index)
    {
        var state = chunk.GetState(layerIndex, index);
        if (state?.Container is null)
        {
            return false;
        }

        if (!Containers.Open(chunk, layerIndex, index, Seed))
        {
            return false;
        }

        if (state.Uuid is not { } uuid)
        {
            return true;
        }

        if (Player.OpenedLootBoxSnapshot.TryGetValue(uuid, out var snapshot))
        {
            state.Container.Slots = snapshot.Items.Select(CloneSlot).ToList();
        }
        else
        {
            Player.OpenedLootBoxSnapshot[uuid] = new LootBoxSnapshot
            {
                Items = state.Container.Slots.Select(CloneSlot).ToList()
            };
        }

        return true;
    }

    public bool PlaceTile(TileVector2D topLeft, int layer, ResourceRef resourceRef, TileResource resource,
        TileDestroySource destroySource, TilePlaceSource placeSource)
    {
        var layerIndex = WorldConstants.LayerToIndex(layer);
        if (layerIndex < 0)
        {
            return false;
        }

        var width = resource.ClampedWidth;
        var height = resource.ClampedHeight;
        for (var cellX = 0; cellX < width; cellX++)
        {
            for (var cellY = 0; cellY < height; cellY++)
            {
                var cell = new TileVector2D(topLeft.X + cellX, topLeft.Y - cellY);
                var chunk = CurrentDimension.Store.GetChunkAt(cell);
                if (chunk is null)
                {
                    continue;
                }

                var index = chunk.IndexOf(cell);
                if (index >= 0)
                {
                    chunk.WriteSingle(layerIndex, index, resourceRef, resource, cellX, cellY, destroySource,
                        placeSource);
                }
            }
        }

        return true;
    }

    /// <summary>
    /// <para>Break the whole multi-tile containing any covered cell.</para>
    /// </summary>
    public bool BreakTileAt(TileVector2D coveredCell, int layer, TileDestroySource destroySource)
    {
        var layerIndex = WorldConstants.LayerToIndex(layer);
        var chunk = CurrentDimension.Store.GetChunkAt(coveredCell);
        if (layerIndex < 0 || chunk is null)
        {
            return false;
        }

        var index = chunk.IndexOf(coveredCell);
        var state = chunk.GetState(layerIndex, index);
        if (state is null)
        {
            return false;
        }

        return BreakTile(state.GetTopLeft(coveredCell), layer, destroySource);
    }

    /// <summary>
    /// <para>Total hardness of the multi-tile containing the cell.</para>
    /// </summary>
    public float GetTileHardness(TileVector2D coveredCell, int layer)
    {
        var layerIndex = WorldConstants.LayerToIndex(layer);
        var chunk = CurrentDimension.Store.GetChunkAt(coveredCell);
        if (layerIndex < 0 || chunk is null)
        {
            return 0;
        }

        var state = chunk.GetState(layerIndex, chunk.IndexOf(coveredCell));
        var tile = chunk.GetTile(layerIndex, chunk.IndexOf(coveredCell));
        return state is null || tile is null ? 0 : TileGeometry.GetHardness(tile.Resource, state.Width, state.Height);
    }

    /// <summary>
    /// <para>Durability cost of breaking the multi-tile containing the cell.</para>
    /// </summary>
    public int GetTileDigCost(TileVector2D coveredCell, int layer)
    {
        var layerIndex = WorldConstants.LayerToIndex(layer);
        var chunk = CurrentDimension.Store.GetChunkAt(coveredCell);
        if (layerIndex < 0 || chunk is null)
        {
            return 0;
        }

        var state = chunk.GetState(layerIndex, chunk.IndexOf(coveredCell));
        var tile = chunk.GetTile(layerIndex, chunk.IndexOf(coveredCell));
        return state is null || tile is null ? 0 : TileGeometry.GetDigCost(tile.Resource, state.Width, state.Height);
    }

    /// <summary>
    /// <para>Break a multi-tile tile at the given top-left cell.</para>
    /// </summary>
    public bool BreakTile(TileVector2D topLeft, int layer, TileDestroySource destroySource)
    {
        var layerIndex = WorldConstants.LayerToIndex(layer);
        if (layerIndex < 0)
        {
            return false;
        }

        var chunk = CurrentDimension.Store.GetChunkAt(topLeft);
        if (chunk is null)
        {
            return false;
        }

        var index = chunk.IndexOf(topLeft);
        var state = chunk.GetState(layerIndex, index);
        if (state is null)
        {
            return false;
        }

        var width = Math.Clamp(state.Width, 1, WorldConstants.ChunkSize);
        var height = Math.Clamp(state.Height, 1, WorldConstants.ChunkSize);

        for (var cellX = 0; cellX < width; cellX++)
        {
            for (var cellY = 0; cellY < height; cellY++)
            {
                var cell = new TileVector2D(topLeft.X + cellX, topLeft.Y - cellY);
                var cellChunk = CurrentDimension.Store.GetChunkAt(cell);
                if (cellChunk is null)
                {
                    continue;
                }

                var cellIndex = cellChunk.IndexOf(cell);
                if (cellIndex >= 0)
                {
                    cellChunk.ClearCell(layerIndex, cellIndex, destroySource);
                }
            }
        }

        return true;
    }

    /// <summary>
    /// <para>Player-facing placement: resolve the anchor, search for attachment support, then place.</para>
    /// </summary>
    public TilePlacementPlan? TryPlacePlayerTile(ResourceRef resourceRef, TileResource resource,
        TileVector2D focusCell, TileVector2D playerCell, bool allowFacingAdjust = true)
    {
        var plan = TilePlacementCalculator.Compute(resource, focusCell, playerCell,
            allowFacingAdjust && resource.AllowDirAdjustAnchor);
        var layer = resource.Layer;
        if (WorldConstants.LayerToIndex(layer) < 0)
        {
            return null;
        }

        var chunk = CurrentDimension.Store.GetChunkAt(plan.TopLeft);
        if (chunk is null)
        {
            return null;
        }

        // Validate every covered cell before the (per-cell, non-transactional) execution stage.
        foreach (var cell in TileGeometry.EnumerateCells(plan.TopLeft, plan.Width, plan.Height))
        {
            if (!WorldConstants.InDimension(CurrentDimension.Resource, cell.X, cell.Y) ||
                CurrentDimension.Store.GetChunkAt(cell) is null)
            {
                return null;
            }
        }

        var search = new AttachmentSearchResult(false, AttachmentDirections.None, new TileVector2D(), 0);
        if (resource.ProbeMask != 0 && CurrentDimension.Store.Attachments is { } attachments)
        {
            var anchor = new TilePlacementAnchor(focusCell, plan.TopLeft, plan.Width, plan.Height);
            search = attachments.Search(chunk, layer, anchor, resource, playerCell);
            if (!search.Found)
            {
                return null;
            }
        }

        if (!PlaceTile(plan.TopLeft, layer, resourceRef, resource, TileDestroySource.Player,
                TilePlaceSource.Player))
        {
            return null;
        }

        if (search.Found)
        {
            SetAttachment(plan.TopLeft, layer, search.Direction, search.Offset, search.SupportLayer);
        }

        return plan;
    }

    /// <summary>
    /// <para>Set attachment metadata on every cell of a placed tile.</para>
    /// </summary>
    public void SetAttachment(TileVector2D topLeft, int layer, AttachmentDirections direction,
        TileVector2D offset, int supportLayer)
    {
        var layerIndex = WorldConstants.LayerToIndex(layer);
        if (layerIndex < 0)
        {
            return;
        }

        var chunk = CurrentDimension.Store.GetChunkAt(topLeft);
        if (chunk is null)
        {
            return;
        }

        var index = chunk.IndexOf(topLeft);
        var state = chunk.GetState(layerIndex, index);
        if (state is null)
        {
            return;
        }

        var width = Math.Clamp(state.Width, 1, WorldConstants.ChunkSize);
        var height = Math.Clamp(state.Height, 1, WorldConstants.ChunkSize);
        for (var x = 0; x < width; x++)
        {
            for (var y = 0; y < height; y++)
            {
                var cell = new TileVector2D(topLeft.X + x, topLeft.Y - y);
                var cellChunk = CurrentDimension.Store.GetChunkAt(cell);
                if (cellChunk is null)
                {
                    continue;
                }

                var cellState = cellChunk.GetState(layerIndex, cellChunk.IndexOf(cell));
                if (cellState is null)
                {
                    continue;
                }

                cellState.AttachmentDirection = direction;
                cellState.AttachmentOffset = offset;
                cellState.AttachmentLayer = supportLayer;
            }
        }
    }

    /// <summary>
    /// <para>Settle loot for the tile at a cell. The container is cleared after drops are emitted.</para>
    /// </summary>
    public void SettleTileLoot(Chunk chunk, int layer, int index)
    {
        var state = chunk.GetState(layer, index);
        var tile = chunk.GetTile(layer, index);
        SettleTileLoot(chunk, layer, index, state, tile);
    }

    /// <summary>
    /// <para>Settle loot from an explicit old state, used when the state has already been replaced.</para>
    /// <para>使用被替换的旧状态结算掉落。用于状态已被新状态覆盖的替换路径。</para>
    /// </summary>
    public void SettleTileLoot(Chunk chunk, int layer, int index, TileState? state, Tile? tile)
    {
        if (state is null || tile is null)
        {
            return;
        }

        var cell = chunk.GetWorldCell(index);
        if (state.Container is not null)
        {
            foreach (var slot in state.Container.Slots)
            {
                if (!slot.IsEmpty && slot.Item is not null)
                {
                    ItemDropped?.Invoke(slot.Item, slot.Count, cell);
                }
            }

            state.Container.Slots.Clear();
        }

        // By default only the centre cell of a multi-tile tile drops resource loot.
        if (!TileGeometry.IsLootCell(tile.Resource, state, cell))
        {
            return;
        }

        var resource = tile.Resource;
        if (!resource.AllowLoot || string.IsNullOrEmpty(resource.LootTarget.ResourceId))
        {
            return;
        }

        var count = 1;
        if (resource.LootScaleByArea)
        {
            count = Math.Max(1, state.Width * state.Height);
        }

        ItemDropped?.Invoke(resource.LootTarget, count, cell);
    }

    /// <summary>
    /// <para>Write the manifest, then every loaded chunk, then the local player.</para>
    /// </summary>
    public bool SaveAll()
    {
        if (_saving)
        {
            // Re-entrant saves are ignored.
            return false;
        }

        _saving = true;
        try
        {
            var success = SaveManifest();
            foreach (var dimension in _dimensions.Values)
            {
                success &= dimension.Store.SaveAll();
            }

            success &= SavePlayer();
            if (!success)
            {
                LogCat.Error("World save completed with errors.");
            }

            return success;
        }
        finally
        {
            _saving = false;
        }
    }

    /// <summary>
    /// <para>Write manifest.bin.</para>
    /// </summary>
    public bool SaveManifest()
    {
        try
        {
            Directory.CreateDirectory(Settings.Directory);
            var manifest = new MapManifestMessage
            {
                SaveFormatVersion = MapManifestMessage.CurrentSaveFormatVersion,
                Name = Settings.Name,
                GameVersionName = GameVersionName,
                GameVersionNumber = GameVersionNumber,
                Seed = Settings.Seed,
                CreateTime = CreateTime,
                GlobalTickCount = (ulong)Math.Max(0, GlobalTick),
                EntityIdIndex = EntityIdIndex
            };
            return WriteAtomic(SavePaths.GetManifestPath(Settings.Directory),
                MemoryPack.MemoryPackSerializer.Serialize(manifest));
        }
        catch (Exception exception)
        {
            LogCat.Error($"Failed to write world manifest: {exception.Message}");
            return false;
        }
    }

    /// <summary>
    /// <para>Write players/local_player.bin.</para>
    /// </summary>
    public bool SavePlayer()
    {
        try
        {
            Directory.CreateDirectory(SavePaths.GetPlayerDirectory(Settings.Directory));
            Player.LastPlayedTime = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
            Player.Visit(CurrentDimension.ResourceRef);
            return WriteAtomic(SavePaths.GetLocalPlayerPath(Settings.Directory),
                MemoryPack.MemoryPackSerializer.Serialize(Player.ToMessage()));
        }
        catch (Exception exception)
        {
            LogCat.Error($"Failed to write player save: {exception.Message}");
            return false;
        }
    }

    /// <summary>
    /// <para>Allocate an entity id. The first returned id is 1.</para>
    /// </summary>
    public uint AllocateEntityId() => ++EntityIdIndex;

    /// <summary>
    /// <para>Optional entity load/save hooks. The engine has no entity store, so these are null by default.</para>
    /// </summary>
    public Func<Chunk, ChunkEntityMessage?>? CollectChunkEntities { get; set; }

    public Action<Chunk, ChunkEntityMessage?>? LoadChunkEntities { get; set; }

    /// <summary>
    /// <para>Unload and save every dimension.</para>
    /// </summary>
    public bool UnloadAll()
    {
        var success = true;
        foreach (var dimension in _dimensions.Values)
        {
            success &= dimension.Store.UnloadAll();
        }

        return success;
    }

    // The chunk store calls these hooks. Entity systems may override behaviour through the events.
    internal void OnChunkLoaded(Chunk chunk)
    {
    }

    internal void OnChunkUnloaded(Chunk chunk)
    {
    }

    internal void OnChunkEntitiesLoaded(Chunk chunk, ChunkEntityMessage? entities)
    {
        LoadChunkEntities?.Invoke(chunk, entities);
    }

    internal ChunkEntityMessage? CollectChunkEntitiesForSave(Chunk chunk) =>
        CollectChunkEntities?.Invoke(chunk);

    internal void BeforeChunkSave(Chunk chunk)
    {
        EntityPersistence?.SyncContainersToTileStates(chunk);
    }

    internal void AfterChunkSaved(Chunk chunk)
    {
        // Invariant: entities are removed only after the chunk save succeeded.
        EntityPersistence?.SaveAndRemoveChunkEntities(chunk);
    }

    /// <summary>
    /// <para>Compute a placement plan with anchor mirroring.</para>
    /// </summary>
    public static TilePlacementPlan ComputePlacement(TileResource resource, TileVector2D focusCell,
        TileVector2D playerCell, bool allowFacingAdjust) =>
        TilePlacementCalculator.Compute(resource, focusCell, playerCell, allowFacingAdjust);

    private static ContainerSlot CloneSlot(ContainerSlot slot)
    {
        ResourceRef? item = null;
        if (slot.Item is not null)
        {
            item = new ResourceRef
            {
                TargetPackId = slot.Item.TargetPackId,
                ResourceType = slot.Item.ResourceType,
                ResourceId = slot.Item.ResourceId
            };
            item.SetPackId(string.IsNullOrEmpty(slot.Item.RequesterPackId)
                ? slot.Item.TargetPackId
                : slot.Item.RequesterPackId);
        }

        return new ContainerSlot { Item = item, Count = slot.Count };
    }

    private static bool WriteAtomic(string path, byte[] bytes)
    {
        var temporaryPath = path + ".tmp";
        try
        {
            var directory = Path.GetDirectoryName(path);
            if (!string.IsNullOrEmpty(directory))
            {
                Directory.CreateDirectory(directory);
            }

            using (var stream = File.Create(temporaryPath))
            {
                stream.Write(bytes);
                stream.Flush(true);
            }

            File.Move(temporaryPath, path, true);
            return true;
        }
        catch (Exception exception)
        {
            LogCat.Error($"Failed to write save file {path}: {exception.Message}");
            return false;
        }
    }

}
