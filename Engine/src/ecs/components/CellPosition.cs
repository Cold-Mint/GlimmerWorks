namespace GlimmerWorks.Engine.Ecs.Components;

/// <summary>
/// <para>CellPosition</para>
/// <para>实体所在的世界瓦片坐标。使用两个 int 而不是 TileVector2D 引用，避免共享可变对象。</para>
/// </summary>
public struct CellPosition : IComponent
{
    public int X;
    public int Y;
}
