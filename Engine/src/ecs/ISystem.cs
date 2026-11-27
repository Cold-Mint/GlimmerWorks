namespace GlimmerWorks.Engine.Ecs;

/// <summary>
/// <para>ISystem</para>
/// <para>ECS 系统。每个逻辑帧由 <see cref="SystemScheduler"/> 按注册顺序调用一次。</para>
/// </summary>
/// <remarks>
/// <para>系统只负责处理数据，不持有跨帧的临时状态；必要的状态放进实体组件。</para>
/// <para>静态瓦片数据（<c>Chunk</c> 内的连续数组）不是实体，系统直接通过 <see cref="SystemContext.Dimension"/> 访问。</para>
/// <para>Systems process data and should not keep frame state; static tile data lives in chunks, not  entities.</para>
/// </remarks>
public interface ISystem
{
    /// <summary>
    /// <para>Run one simulation step.</para>
    /// </summary>
    void Update(in SystemContext context);
}
