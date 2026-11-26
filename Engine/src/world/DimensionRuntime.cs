using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Attachments;
using GlimmerWorks.Engine.World.Generation;
using GlimmerWorks.Engine.World.Lighting;
using GlimmerWorks.Engine.World.Noise;
using GlimmerWorks.Engine.World.Storage;
using GlimmerWorks.Engine.World.Streaming;
using GlimmerWorks.Engine.World.Systems;
using GlimmerWorks.Engine.World.Terrain;
using GlimmerWorks.Engine.World.Tiles;

namespace GlimmerWorks.Engine.World;

/// <summary>
/// <para>DimensionRuntime</para>
/// </summary>
public sealed class DimensionRuntime
{
    /// <summary>
    /// <para>Normalized day length in seconds at time_flow = 1.</para>
    /// </summary>
    public const double DayLengthSeconds = 600.0;

    private LightColor _lastAmbientBacklight;
    private LightColor _lastAmbientSkylight;

    public DimensionRuntime(
        DimensionResource resource,
        long worldSeed,
        DataRepository dataRepository,
        ITileRegistry tiles,
        IChunkStorage storage)
    {
        Resource = resource;
        WorldSeed = worldSeed;
        DataRepository = dataRepository;
        Tiles = tiles;
        ResourceRef = ResourceRef.Create("dimension", resource.PackId, resource.ResourceId);
        ResourceRef.SetPackId(resource.PackId);

        var noise = new DimensionNoiseSet(
            ResolveNoise(resource.ElevationNoise, 1, dataRepository, worldSeed),
            ResolveNoise(resource.HumidityNoise, 2, dataRepository, worldSeed),
            ResolveNoise(resource.TemperatureNoise, 3, dataRepository, worldSeed),
            ResolveNoise(resource.WeirdnessNoise, 4, dataRepository, worldSeed),
            ResolveNoise(resource.ErosionNoise, 5, dataRepository, worldSeed));

        Biomes = resource.Biomes
            .Select(dataRepository.Get<BiomeResource>)
            .Where(static biome => biome is not null)
            .Select(static biome => biome!)
            .ToList();
        Structures = dataRepository.GetResourceList<StructureResource>().ToList();

        Terrain = new TerrainGenerator(resource, ResourceRef, worldSeed, dataRepository, noise, Biomes, Structures);
        Pool = new TileInstancePool(tiles);
        Generator = new ChunkGenerator(tiles, resource, ResourceRef, worldSeed, Terrain.Climate, dataRepository,
            Pool);
        Store = new ChunkStore(resource.Id, tiles, Pool, Terrain, Generator, storage);
        Light = new LightBuffer();
        Attachments = new AttachmentSystem(Store);
        Growth = new GrowthSystem(Store);
        Containers = new ContainerSystem(dataRepository);
        Streaming = new ChunkStreamingSystem(Store);

        Store.Light = Light;
        Store.Attachments = Attachments;
        Store.Growth = Growth;
        Light.ChunkLookup = Store.GetChunk;
        Light.ChunkSnapshotProvider = Store.GetLoadedChunks;

        CurrentTime = Math.Clamp(resource.InitialTime, 0.0F, 1.0F);
        UpdateAmbient(force: true);
    }

    public DimensionResource Resource { get; }

    public ResourceRef ResourceRef { get; }

    public long WorldSeed { get; }

    public DataRepository DataRepository { get; }

    public ITileRegistry Tiles { get; }

    public IReadOnlyList<BiomeResource> Biomes { get; }

    public IReadOnlyList<StructureResource> Structures { get; }

    public TileInstancePool Pool { get; }

    public TerrainGenerator Terrain { get; }

    public ChunkGenerator Generator { get; }

    public ChunkStore Store { get; }

    public LightBuffer Light { get; }

    public AttachmentSystem Attachments { get; }

    public GrowthSystem Growth { get; }

    public ContainerSystem Containers { get; }

    public ChunkStreamingSystem Streaming { get; }

    /// <summary>
    /// <para>Normalized time in [0,1).</para>
    /// </summary>
    public double CurrentTime { get; private set; }

    /// <summary>
    /// <para>Advance the dimension clock and refresh ambient light when it changed.</para>
    /// </summary>
    public void AdvanceTime(long deltaMs)
    {
        if (Resource.TimeFlow == 0 || deltaMs == 0)
        {
            return;
        }

        CurrentTime = (CurrentTime + deltaMs / 1000.0 * Resource.TimeFlow / DayLengthSeconds) % 1.0;
        if (CurrentTime < 0)
        {
            CurrentTime += 1.0;
        }

        UpdateAmbient();
    }

    /// <summary>
    /// <para>Refresh ambient colors from keyframes.</para>
    /// </summary>
    public void UpdateAmbient(bool force = false)
    {
        var backlight = SampleKeyframes(Resource.BacklightKeyframes, CurrentTime) ??
                        LightColor.FromRgba(255, 255, 255);
        var skylight = SampleKeyframes(Resource.SkylightKeyframes, CurrentTime) ?? LightColor.FromRgba(0, 0, 0);

        if (!force && backlight == _lastAmbientBacklight && skylight == _lastAmbientSkylight)
        {
            return;
        }

        _lastAmbientBacklight = backlight;
        _lastAmbientSkylight = skylight;
        Light.SetAmbient(backlight, skylight);
    }

    private static LightColor? SampleKeyframes(List<LightKeyframeResource> keyframes, double time)
    {
        if (keyframes.Count == 0)
        {
            return null;
        }

        var ordered = keyframes.OrderBy(static frame => frame.Time).ToList();
        var normalized = (float)(time - Math.Floor(time));

        if (normalized <= ordered[0].Time)
        {
            return LightColor.FromRgba(ordered[0].R, ordered[0].G, ordered[0].B, ordered[0].A);
        }

        for (var i = 0; i < ordered.Count; i++)
        {
            var current = ordered[i];
            var next = ordered[(i + 1) % ordered.Count];
            var end = i + 1 < ordered.Count ? next.Time : next.Time + 1.0F;
            if (normalized < current.Time || normalized > end)
            {
                continue;
            }

            var span = Math.Max(0.0001F, end - current.Time);
            var t = (normalized - current.Time) / span;
            return LightColor.Lerp(
                LightColor.FromRgba(current.R, current.G, current.B, current.A),
                LightColor.FromRgba(next.R, next.G, next.B, next.A),
                t);
        }

        var last = ordered[^1];
        return LightColor.FromRgba(last.R, last.G, last.B, last.A);
    }

    private static NoiseSampler ResolveNoise(ResourceRef reference, int salt, DataRepository dataRepository,
        long worldSeed)
    {
        if (string.IsNullOrEmpty(reference.ResourceId))
        {
            return NoiseSampler.CreateDefault(worldSeed, salt);
        }

        var noiseResource = dataRepository.Get<NoiseResource>(reference);
        if (noiseResource is null)
        {
            LogCat.Warn($"Noise resource missing: {reference.TargetPackId}:{reference.ResourceId}; using fallback.");
            return NoiseSampler.CreateDefault(worldSeed, salt);
        }

        return NoiseSampler.Create(noiseResource, worldSeed);
    }
}
