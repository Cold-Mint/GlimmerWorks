namespace GlimmerWorks.Engine.World.Ecs;

public interface ISystem
{
    /// <summary>
    /// <para>DependentComponents</para>
    /// <para>依赖那些类型的组建</para>
    /// </summary>
    /// <returns></returns>
    public IReadOnlyList<Type> DependentComponents { get; }

    /// <summary>
    /// <para>OnActivateStatusChanged</para>
    /// <para>当系统的激活状态改变时调用。</para>
    /// </summary>
    /// <param name="activate">
    /// <para>true when the system was activated; false when it was disabled.</para>
    /// <para>true 表示系统被激活，false 表示系统被禁用。</para>
    /// </param>
    public void OnActivateStatusChanged(bool activate);

    /// <summary>
    /// <para>OnTick</para>
    /// <para>当触发Tick时</para>
    /// </summary>
    /// <param name="deltaMs"></param>
    /// <param name="globalTick"></param>
    public void OnTick(long deltaMs, long globalTick);

}