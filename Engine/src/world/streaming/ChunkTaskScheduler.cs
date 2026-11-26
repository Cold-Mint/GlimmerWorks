using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.World.Storage;

namespace GlimmerWorks.Engine.World.Streaming;

/// <summary>
/// <para>ChunkTaskType</para>
/// </summary>
public enum ChunkTaskType
{
    Load,
    Unload
}

/// <summary>
/// <para>ChunkTask</para>
/// </summary>
public readonly record struct ChunkTask(ChunkTaskType Type, ChunkVertexVector2D Position, long Sequence);

/// <summary>
/// <para>WorldRect</para>
/// </summary>
public readonly record struct WorldRect(float Left, float Bottom, float Right, float Top)
{
    public float Width => Right - Left;

    public float Height => Top - Bottom;

    public WorldRect Expand(float amount) =>
        new(Left - amount, Bottom - amount, Right + amount, Top + amount);
}

/// <summary>
/// <para>CameraView</para>
/// </summary>
public readonly record struct CameraView(WorldVector2D Position, WorldVector2D Size, float Zoom)
{
    public WorldRect GetWorldRect()
    {
        var zoom = MathF.Max(0.0001F, Zoom);
        var width = Size.X / zoom;
        var height = Size.Y / zoom;
        return new WorldRect(
            Position.X - width * 0.5F,
            Position.Y - height * 0.5F,
            Position.X + width * 0.5F,
            Position.Y + height * 0.5F);
    }
}

/// <summary>
/// <para>ChunkTaskScheduler</para>
/// </summary>
public sealed class ChunkTaskScheduler
{
    private readonly Dictionary<ChunkVertexVector2D, ChunkTask> _pending = [];
    private readonly object _lock = new();
    private long _sequence;
    private bool _dispatching;
    private CameraView? _lastCamera;

    /// <summary>
    /// <para>Number of pending tasks.</para>
    /// </summary>
    public int PendingCount
    {
        get
        {
            lock (_lock)
            {
                return _pending.Count;
            }
        }
    }

    /// <summary>
    /// <para>Whether a task delivery is in progress.</para>
    /// </summary>
    public bool IsDispatching
    {
        get
        {
            lock (_lock)
            {
                return _dispatching;
            }
        }
    }

    /// <summary>
    /// <para>Reset the last camera snapshot so the next scan always runs.</para>
    /// </summary>
    public void InvalidateCamera()
    {
        lock (_lock)
        {
            _lastCamera = null;
        }
    }

    /// <summary>
    /// <para>Scan the viewport and submit load/unload tasks.</para>
    /// </summary>
    public void Scan(CameraView camera, Func<ChunkVertexVector2D, bool> isLoaded, int preloadRadiusChunks = 4,
        Func<IReadOnlyList<ChunkVertexVector2D>>? loadedPositions = null)
    {
        lock (_lock)
        {
            if (_lastCamera == camera)
            {
                return;
            }

            _lastCamera = camera;
        }

        var viewport = camera.GetWorldRect().Expand(preloadRadiusChunks * (float)WorldConstants.ChunkPixelSize);
        var startTile = WorldConstants.WorldToTile(new WorldVector2D(viewport.Left, viewport.Bottom));
        var endTile = WorldConstants.WorldToTile(new WorldVector2D(viewport.Right, viewport.Top));
        var startChunk = WorldConstants.GetChunkVertex(startTile);
        var endChunk = WorldConstants.GetChunkVertex(endTile);

        var tasks = new List<ChunkTask>();
        for (var chunkY = startChunk.Y; chunkY <= endChunk.Y; chunkY += WorldConstants.ChunkSize)
        {
            for (var chunkX = startChunk.X; chunkX <= endChunk.X; chunkX += WorldConstants.ChunkSize)
            {
                var position = new ChunkVertexVector2D(chunkX, chunkY);
                var loaded = isLoaded(position);
                if (!loaded)
                {
                    tasks.Add(new ChunkTask(ChunkTaskType.Load, position, 0));
                }
            }
        }

        if (loadedPositions is not null)
        {
            foreach (var position in loadedPositions())
            {
                if (position.X < startChunk.X || position.X > endChunk.X ||
                    position.Y < startChunk.Y || position.Y > endChunk.Y)
                {
                    tasks.Add(new ChunkTask(ChunkTaskType.Unload, position, 0));
                }
            }
        }

        Submit(tasks);
    }

    /// <summary>
    /// <para>Submit tasks. A newer task of the same chunk replaces the older one.</para>
    /// </summary>
    public void Submit(IEnumerable<ChunkTask> tasks)
    {
        lock (_lock)
        {
            foreach (var task in tasks)
            {
                _pending[task.Position] = task with { Sequence = ++_sequence };
            }
        }
    }

    /// <summary>
    /// <para>Submit unload tasks for every currently loaded chunk.</para>
    /// </summary>
    public void SubmitUnloadAll(IEnumerable<ChunkVertexVector2D> loadedPositions)
    {
        Submit(loadedPositions.Select(static position => new ChunkTask(ChunkTaskType.Unload, position, 0)));
    }

    /// <summary>
    /// <para>Take all pending tasks sorted by squared distance to the camera chunk, then block further delivery until EndDispatch.</para>
    /// </summary>
    public bool TryBeginDispatch(WorldVector2D cameraPosition, out List<ChunkTask> tasks)
    {
        lock (_lock)
        {
            tasks = [];
            if (_dispatching || _pending.Count == 0)
            {
                return false;
            }

            _dispatching = true;
            var cameraTile = WorldConstants.WorldToTile(cameraPosition);
            var cameraChunk = WorldConstants.GetChunkVertex(cameraTile);
            tasks = _pending.Values
                .OrderBy(task => DistanceSquared(task.Position, cameraChunk))
                .ThenBy(static task => task.Sequence)
                .ToList();
            _pending.Clear();
            return true;
        }
    }

    /// <summary>
    /// <para>Finish a delivery and allow the next one.</para>
    /// </summary>
    public void EndDispatch()
    {
        lock (_lock)
        {
            _dispatching = false;
        }
    }

    /// <summary>
    /// <para>Drop pending tasks without executing them. Used when leaving a dimension.</para>
    /// <para>丢弃待执行任务且不执行。离开维度时使用。</para>
    /// </summary>
    public void ClearPending()
    {
        lock (_lock)
        {
            _pending.Clear();
        }
    }

    private static long DistanceSquared(ChunkVertexVector2D position, ChunkVertexVector2D cameraChunk)
    {
        var dx = position.X - cameraChunk.X;
        var dy = position.Y - cameraChunk.Y;
        return (long)dx * dx + (long)dy * dy;
    }
}

/// <summary>
/// <para>ChunkStreamingSystem</para>
/// </summary>
public sealed class ChunkStreamingSystem(ChunkStore store)
{
    public ChunkStore Store { get; } = store;

    public ChunkTaskScheduler Scheduler { get; } = new();

    public int PreloadRadiusChunks { get; set; } = 4;

    public bool Asynchronous { get; set; } = true;

    /// <summary>
    /// <para>True after the first task batch has finished (or the first synchronous load completed).</para>
    /// </summary>
    public bool HasCompletedInitialLoad { get; private set; }

    public event Action? InitialLoadCompleted;

    /// <summary>
    /// <para>Reset the first-load marker, e.g. when leaving a dimension.</para>
    /// </summary>
    public void ResetInitialLoad()
    {
        HasCompletedInitialLoad = false;
    }

    public bool IsInitialLoadComplete => HasCompletedInitialLoad;

    /// <summary>
    /// <para>Scan and optionally deliver tasks.</para>
    /// </summary>
    public void Tick(CameraView camera)
    {
        Scheduler.Scan(camera, position => Store.GetChunk(position) is not null, PreloadRadiusChunks,
            Store.GetLoadedPositions);
        TryDispatch(camera.Position);
    }

    /// <summary>
    /// <para>Try to deliver a pending batch.</para>
    /// </summary>
    public bool TryDispatch(WorldVector2D cameraPosition)
    {
        if (!Scheduler.TryBeginDispatch(cameraPosition, out var tasks))
        {
            return false;
        }

        if (!Asynchronous)
        {
            Execute(tasks);
            Scheduler.EndDispatch();
            return true;
        }

        ThreadPool.QueueUserWorkItem(_ =>
        {
            try
            {
                Execute(tasks);
            }
            finally
            {
                Scheduler.EndDispatch();
            }
        });
        return true;
    }

    /// <summary>
    /// <para>Execute pending tasks on the calling thread. Useful for tests and for deterministic stepping.</para>
    /// </summary>
    public void ExecutePendingInline(WorldVector2D cameraPosition)
    {
        if (!Scheduler.TryBeginDispatch(cameraPosition, out var tasks))
        {
            return;
        }

        try
        {
            Execute(tasks);
        }
        finally
        {
            Scheduler.EndDispatch();
        }
    }

    /// <summary>
    /// <para>Queue unload tasks for every loaded chunk, e.g. when switching dimensions.</para>
    /// </summary>
    public void RequestUnloadAll()
    {
        Scheduler.SubmitUnloadAll(Store.GetLoadedPositions());
    }

    private void Execute(List<ChunkTask> tasks)
    {
        foreach (var task in tasks)
        {
            try
            {
                switch (task.Type)
                {
                    case ChunkTaskType.Load:
                        Store.LoadChunk(task.Position);
                        break;
                    case ChunkTaskType.Unload:
                        Store.UnloadChunk(task.Position);
                        break;
                }
            }
            catch (Exception exception)
            {
                LogCat.Error($"Chunk task {task.Type} at {task.Position.X},{task.Position.Y} failed: " +
                             exception.Message);
            }
        }

        if (!HasCompletedInitialLoad && tasks.Any(static task => task.Type == ChunkTaskType.Load))
        {
            HasCompletedInitialLoad = true;
            InitialLoadCompleted?.Invoke();
        }
    }
}
