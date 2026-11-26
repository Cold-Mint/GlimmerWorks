using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.World.Storage;
using GlimmerWorks.Engine.World.Tiles;

namespace GlimmerWorks.Engine.World.Lighting;

/// <summary>
/// <para>LightColor</para>
/// </summary>
public readonly record struct LightColor(float R, float G, float B, float A)
{
    public static readonly LightColor Black = new(0, 0, 0, 0);
    public static readonly LightColor White = new(1, 1, 1, 1);

    public static LightColor FromRgba(int r, int g, int b, int a = 255) => new(
        Math.Clamp(r, 0, 255) / 255.0F,
        Math.Clamp(g, 0, 255) / 255.0F,
        Math.Clamp(b, 0, 255) / 255.0F,
        Math.Clamp(a, 0, 255) / 255.0F);

    public static LightColor FromColor(Color color) => FromRgba(color.R, color.G, color.B, color.A);

    /// <summary>
    /// <para>Blocking strength from the alpha channel.</para>
    /// </summary>
    public float Blocking => A;

    public float Luminance => R * 0.2126F + G * 0.7152F + B * 0.0722F;

    public static LightColor operator +(LightColor left, LightColor right) =>
        new(left.R + right.R, left.G + right.G, left.B + right.B, left.A + right.A);

    public static LightColor operator *(LightColor color, float scalar) =>
        new(color.R * scalar, color.G * scalar, color.B * scalar, color.A * scalar);

    public static LightColor operator *(float scalar, LightColor color) => color * scalar;

    public LightColor Clamped() => new(
        Math.Clamp(R, 0, 1), Math.Clamp(G, 0, 1), Math.Clamp(B, 0, 1), Math.Clamp(A, 0, 1));

    public static LightColor Lerp(LightColor from, LightColor to, float t)
    {
        t = Math.Clamp(t, 0.0F, 1.0F);
        return new LightColor(
            from.R + (to.R - from.R) * t,
            from.G + (to.G - from.G) * t,
            from.B + (to.B - from.B) * t,
            from.A + (to.A - from.A) * t);
    }
}

/// <summary>
/// <para>LightCell</para>
/// </summary>
public struct LightCell
{
    public LightColor Source;
    public LightColor BacklightMask;
    public LightColor SideMask;
    public LightColor StaticContribution;
    public LightColor DynamicContribution;
    public LightColor Final;
    public long Revision;
}

/// <summary>
/// <para>DynamicLight</para>
/// </summary>
public sealed record DynamicLight(TileVector2D Cell, LightColor Color, float Radius);

/// <summary>
/// <para>LightBuffer</para>
/// </summary>
/// <remarks>
/// <para>Backlight masks apply to every layer. Side masks and point lights apply to the foreground layer only.</para>
/// </remarks>
public sealed class LightBuffer
{
    private static readonly (int X, int Y)[] Directions =
    [
        (0, 1), (1, 0), (0, -1), (-1, 0),
        (1, 1), (1, -1), (-1, -1), (-1, 1)
    ];

    private readonly Dictionary<ChunkVertexVector2D, ChunkLightData> _chunks = [];
    private readonly Dictionary<int, DynamicLight> _dynamicLights = [];
    private readonly object _lock = new();

    private bool _staticDirty = true;
    private bool _dynamicDirty = true;
    private bool _ambientDirty = true;

    /// <summary>
    /// <para>Lookup used to trace light across loaded chunks.</para>
    /// </summary>
    public Func<ChunkVertexVector2D, Chunk?>? ChunkLookup { get; set; }

    /// <summary>
    /// <para>Snapshot provider for static light collection.</para>
    /// </summary>
    public Func<IReadOnlyList<Chunk>>? ChunkSnapshotProvider { get; set; }

    /// <summary>
    /// <para>Ambient backlight applied to every layer.</para>
    /// </summary>
    public LightColor AmbientBacklight { get; private set; } = LightColor.Black;

    /// <summary>
    /// <para>Ambient sky light.</para>
    /// </summary>
    public LightColor AmbientSkylight { get; private set; } = LightColor.Black;

    /// <summary>
    /// <para>Monotonic buffer revision.</para>
    /// </summary>
    public long Revision { get; private set; }

    /// <summary>
    /// <para>Number of chunks with light data.</para>
    /// </summary>
    public int ChunkCount
    {
        get
        {
            lock (_lock)
            {
                return _chunks.Count;
            }
        }
    }

    /// <summary>
    /// <para>Set the ambient light colors. Only the final colors are rebuilt.</para>
    /// </summary>
    public void SetAmbient(LightColor backlight, LightColor skylight)
    {
        AmbientBacklight = backlight;
        AmbientSkylight = skylight;
        _ambientDirty = true;
        Revision++;
    }

    /// <summary>
    /// <para>Allocate light data for a newly loaded chunk.</para>
    /// </summary>
    public void OnChunkLoaded(Chunk chunk)
    {
        lock (_lock)
        {
            if (!_chunks.ContainsKey(chunk.Position))
            {
                _chunks[chunk.Position] = new ChunkLightData();
            }
        }

        _staticDirty = true;
        Revision++;
    }

    /// <summary>
    /// <para>Mark static lighting dirty after a tile change.</para>
    /// </summary>
    public void OnChunkChanged(Chunk chunk, int layer, int index, Tile? oldTile, Tile? newTile,
        TileDestroySource destroySource)
    {
        _staticDirty = true;
        Revision++;
    }

    /// <summary>
    /// <para>Remove light data for an unloaded chunk.</para>
    /// </summary>
    public void ClearChunk(Chunk chunk)
    {
        lock (_lock)
        {
            _chunks.Remove(chunk.Position);
        }

        _staticDirty = true;
        Revision++;
    }

    /// <summary>
    /// <para>Add or update a dynamic light by unique ID. Does not trigger a static rebuild.</para>
    /// </summary>
    public void SetDynamicLight(int id, TileVector2D cell, LightColor color, float radius)
    {
        lock (_lock)
        {
            _dynamicLights[id] = new DynamicLight(cell, color, Math.Max(0.0F, radius));
        }

        _dynamicDirty = true;
        Revision++;
    }

    /// <summary>
    /// <para>Remove a dynamic light by unique ID.</para>
    /// </summary>
    public void RemoveDynamicLight(int id)
    {
        lock (_lock)
        {
            _dynamicLights.Remove(id);
        }

        _dynamicDirty = true;
        Revision++;
    }

    /// <summary>
    /// <para>Rebuild dirty light data. Call this before rendering.</para>
    /// </summary>
    public void Refresh()
    {
        if (!_staticDirty && !_dynamicDirty && !_ambientDirty)
        {
            return;
        }

        if (_staticDirty)
        {
            RebuildStatic();
            _staticDirty = false;
            _ambientDirty = true;
        }

        if (_dynamicDirty)
        {
            RebuildDynamic();
            _dynamicDirty = false;
            _ambientDirty = true;
        }

        if (_ambientDirty)
        {
            RebuildFinal();
            _ambientDirty = false;
        }

        Revision++;
    }

    /// <summary>
    /// <para>Get a copy of a light cell.</para>
    /// </summary>
    public LightCell? GetCell(TileVector2D worldCell, int layer)
    {
        var layerIndex = WorldConstants.LayerToIndex(layer);
        var chunkPosition = WorldConstants.GetChunkVertex(worldCell);
        var local = WorldConstants.GetChunkRelative(worldCell);
        var index = WorldConstants.ChunkIndex(local);

        lock (_lock)
        {
            if (layerIndex < 0 || !_chunks.TryGetValue(chunkPosition, out var data))
            {
                return null;
            }

            return data.Cells[layerIndex][index];
        }
    }

    /// <summary>
    /// <para>Get the final color of a cell, or black when no data exists.</para>
    /// </summary>
    public LightColor GetFinalColor(TileVector2D worldCell, int layer) =>
        GetCell(worldCell, layer)?.Final ?? LightColor.Black;

    private void RebuildStatic()
    {
        List<Chunk> chunks;
        lock (_lock)
        {
            foreach (var data in _chunks.Values)
            {
                data.ResetStatic();
            }

            chunks = ChunkSnapshotProvider?.Invoke().ToList() ?? [];
        }

        var staticLights = new List<DynamicLight>();
        foreach (var chunk in chunks)
        {
            ChunkLightData? data;
            lock (_lock)
            {
                data = _chunks.GetValueOrDefault(chunk.Position);
            }

            if (data is null)
            {
                continue;
            }

            CollectChunkSources(chunk, data, staticLights);
            ApplySkyTransmittance(chunk, data);
        }

        foreach (var light in staticLights)
        {
            Propagate(light, staticContribution: true);
        }
    }

    private void CollectChunkSources(Chunk chunk, ChunkLightData data, List<DynamicLight> staticLights)
    {
        for (var layerIndex = 0; layerIndex < WorldConstants.LayerCount; layerIndex++)
        {
            for (var index = 0; index < WorldConstants.ChunkCellCount; index++)
            {
                var state = chunk.GetState(layerIndex, index);
                var tile = chunk.GetTile(layerIndex, index);
                if (state is null || tile is null)
                {
                    continue;
                }

                var resource = tile.Resource;
                var cell = data.Cells[layerIndex][index];
                cell.BacklightMask = LightColor.FromRgba(
                    resource.BacklightMaskR, resource.BacklightMaskG,
                    resource.BacklightMaskB, resource.BacklightMaskA);
                cell.SideMask = layerIndex == 2
                    ? LightColor.FromRgba(resource.SideMaskR, resource.SideMaskG, resource.SideMaskB,
                        resource.SideMaskA)
                    : LightColor.Black;

                if (layerIndex == 2 && resource.EmitsLight)
                {
                    var color = LightColor.FromRgba(resource.LightR, resource.LightG, resource.LightB) *
                                resource.LightIntensity;
                    cell.Source = color;

                    // Lights are per cell by default. A resource may opt into origin-only emission.
                    if (!resource.LightOriginOnly || state.IsOriginCell)
                    {
                        staticLights.Add(new DynamicLight(chunk.GetWorldCell(index), color,
                            Math.Max(0.0F, resource.LightRadius)));
                    }
                }

                data.Cells[layerIndex][index] = cell;
            }
        }
    }

    private static void ApplySkyTransmittance(Chunk chunk, ChunkLightData data)
    {
        for (var x = 0; x < WorldConstants.ChunkSize; x++)
        {
            var transmittance = 1.0F;
            for (var y = WorldConstants.ChunkSize - 1; y >= 0; y--)
            {
                var index = WorldConstants.ChunkIndex(x, y);
                var mask = data.Cells[2][index].BacklightMask;
                transmittance *= 1.0F - Math.Clamp(mask.Blocking, 0.0F, 1.0F);
                for (var layerIndex = 0; layerIndex < WorldConstants.LayerCount; layerIndex++)
                {
                    var cell = data.Cells[layerIndex][index];
                    cell.StaticContribution += new LightColor(transmittance, transmittance, transmittance, 0);
                    data.Cells[layerIndex][index] = cell;
                }
            }
        }
    }

    private void RebuildDynamic()
    {
        lock (_lock)
        {
            foreach (var data in _chunks.Values)
            {
                data.ResetDynamic();
            }
        }

        List<DynamicLight> lights;
        lock (_lock)
        {
            lights = _dynamicLights.Values.ToList();
        }

        foreach (var light in lights)
        {
            Propagate(light, staticContribution: false);
        }
    }

    private void Propagate(DynamicLight light, bool staticContribution)
    {
        if (light.Radius <= 0 && light.Color.Luminance <= 0)
        {
            return;
        }

        var queue = new Queue<(int X, int Y, LightColor Color)>();
        var visited = new HashSet<(int X, int Y)>();
        queue.Enqueue((light.Cell.X, light.Cell.Y, light.Color));
        visited.Add((light.Cell.X, light.Cell.Y));

        var radiusSquared = Math.Max(1.0F, light.Radius * light.Radius);
        var steps = 0;
        var maxSteps = (int)(light.Radius * 3) + 1;

        while (queue.Count > 0 && steps++ < 10000)
        {
            var (x, y, color) = queue.Dequeue();
            ApplyContribution(new TileVector2D(x, y), color, staticContribution);

            foreach (var (dx, dy) in Directions)
            {
                var nx = x + dx;
                var ny = y + dy;
                if (!visited.Add((nx, ny)))
                {
                    continue;
                }

                var distanceSquared = (nx - light.Cell.X) * (nx - light.Cell.X) +
                                      (ny - light.Cell.Y) * (ny - light.Cell.Y);
                if (distanceSquared > radiusSquared || distanceSquared > maxSteps * maxSteps)
                {
                    continue;
                }

                var blocking = GetForegroundBlocking(new TileVector2D(nx, ny));
                var attenuation = 0.86F * (1.0F - Math.Clamp(blocking, 0.0F, 1.0F));
                var next = color * attenuation;
                if (next.Luminance <= 0.01F)
                {
                    continue;
                }

                queue.Enqueue((nx, ny, next));
            }
        }
    }

    private float GetForegroundBlocking(TileVector2D cell)
    {
        var chunk = ChunkLookup?.Invoke(WorldConstants.GetChunkVertex(cell));
        if (chunk is null)
        {
            return 1.0F;
        }

        var index = chunk.IndexOf(cell);
        if (index < 0)
        {
            return 1.0F;
        }

        var tile = chunk.GetTile(2, index);
        if (tile is null)
        {
            return 0.0F;
        }

        return Math.Max(tile.Resource.BacklightMaskA, tile.Resource.SideMaskA) / 255.0F;
    }

    private void ApplyContribution(TileVector2D worldCell, LightColor color, bool staticContribution)
    {
        var chunkPosition = WorldConstants.GetChunkVertex(worldCell);
        var local = WorldConstants.GetChunkRelative(worldCell);
        var index = WorldConstants.ChunkIndex(local);

        lock (_lock)
        {
            if (!_chunks.TryGetValue(chunkPosition, out var data))
            {
                return;
            }

            var cell = data.Cells[2][index];
            if (staticContribution)
            {
                cell.StaticContribution += color;
            }
            else
            {
                cell.DynamicContribution += color;
            }

            data.Cells[2][index] = cell;
        }
    }

    private void RebuildFinal()
    {
        lock (_lock)
        {
            foreach (var data in _chunks.Values)
            {
                for (var layerIndex = 0; layerIndex < WorldConstants.LayerCount; layerIndex++)
                {
                    for (var index = 0; index < WorldConstants.ChunkCellCount; index++)
                    {
                        var cell = data.Cells[layerIndex][index];
                        cell.Final = (AmbientBacklight + AmbientSkylight +
                                      cell.StaticContribution + cell.DynamicContribution).Clamped();
                        cell.Revision = Revision + 1;
                        data.Cells[layerIndex][index] = cell;
                    }
                }
            }
        }
    }
}

internal sealed class ChunkLightData
{
    public LightCell[][] Cells { get; } =
    [
        new LightCell[WorldConstants.ChunkCellCount],
        new LightCell[WorldConstants.ChunkCellCount],
        new LightCell[WorldConstants.ChunkCellCount]
    ];

    public void ResetStatic()
    {
        for (var layer = 0; layer < WorldConstants.LayerCount; layer++)
        {
            for (var index = 0; index < WorldConstants.ChunkCellCount; index++)
            {
                var cell = Cells[layer][index];
                cell.Source = LightColor.Black;
                cell.BacklightMask = LightColor.Black;
                cell.SideMask = LightColor.Black;
                cell.StaticContribution = LightColor.Black;
                Cells[layer][index] = cell;
            }
        }
    }

    public void ResetDynamic()
    {
        for (var layer = 0; layer < WorldConstants.LayerCount; layer++)
        {
            for (var index = 0; index < WorldConstants.ChunkCellCount; index++)
            {
                var cell = Cells[layer][index];
                cell.DynamicContribution = LightColor.Black;
                Cells[layer][index] = cell;
            }
        }
    }
}
