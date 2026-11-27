namespace GlimmerWorks.Engine.Ecs.Systems;

/// <summary>
/// <para>TimeSystem</para>
/// <para>推进维度时钟并刷新环境光。必须是调度器中的第一个系统。</para>
/// </summary>
public sealed class TimeSystem : ISystem
{
    public void Update(in SystemContext context)
    {
        context.Dimension.AdvanceTime(context.DeltaMs);
    }
}
