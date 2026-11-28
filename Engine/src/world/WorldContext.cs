using GlimmerWorks.Engine.World.Ecs;
using GlimmerWorks.Engine.World.generator;

namespace GlimmerWorks.Engine.World;

/// <summary>
/// <para>WorldContext</para>
/// <para>世界上下文</para>
/// </summary>
public class WorldContext
{
    /// <summary>
    /// <para>DimensionRuntime</para>
    /// <para>维度运行时</para>
    /// </summary>
    public DimensionRuntime DimensionRuntime  { get; } = new();

    /// <summary>
    /// <para>WorldSettings</para>
    /// <para>世界设置</para>
    /// </summary>
    public WorldSettings WorldSettings  { get; } = new();

    /// <summary>
    /// <para>DimensionGenerator</para>
    /// <para>维度生成器</para>
    /// </summary>
    private DimensionGenerator DimensionGenerator  { get; } = new();

    /// <summary>
    /// <para>EntityManager</para>
    /// <para>实体管理器</para>
    /// </summary>
    public EntityManager EntityManager { get; } = new();

    /// <summary>
    /// <para>System scheduler</para>
    /// <para>系统调度器</para>
    /// </summary>
    public SystemScheduler SystemScheduler { get; } = new();
    
}