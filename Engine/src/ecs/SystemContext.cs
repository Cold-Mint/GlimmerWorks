using GlimmerWorks.Engine.World;

namespace GlimmerWorks.Engine.Ecs;

/// <summary>
/// <para>SystemContext</para>
/// <para>传给每个 <see cref="ISystem"/> 的单帧上下文。</para>
/// </summary>
public readonly struct SystemContext
{
    public SystemContext(EcsWorld ecs, DimensionRuntime dimension, long deltaMs, long tick)
    {
        Ecs = ecs;
        Dimension = dimension;
        DeltaMs = deltaMs;
        Tick = tick;
    }

    /// <summary>
    /// <para>Entity store of the current dimension.</para>
    /// </summary>
    public EcsWorld Ecs { get; }

    /// <summary>
    /// <para>The dimension being simulated.</para>
    /// </summary>
    public DimensionRuntime Dimension { get; }

    /// <summary>
    /// <para>Elapsed milliseconds since the previous tick. May be larger than the target frame time.</para>
    /// </summary>
    public long DeltaMs { get; }

    /// <summary>
    /// <para>Monotonic world tick counter.</para>
    /// </summary>
    public long Tick { get; }
}
