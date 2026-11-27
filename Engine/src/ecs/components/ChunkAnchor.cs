namespace GlimmerWorks.Engine.Ecs.Components;

/// <summary>
/// <para>ChunkAnchor</para>
/// <para>实体所属的区块顶点。区块保存/卸载时按它分组，决定实体写进哪个存档文件。</para>
/// </summary>
public struct ChunkAnchor : IComponent
{
    public int X;
    public int Y;
}
