namespace GlimmerWorks.Engine.World;

/// <summary>
/// <para>WorldSettings</para>
/// <para>世界设置</para>
/// </summary>
public class WorldSettings
{
    /// <summary>
    /// <para>WorldName</para>
    /// <para>世界名称</para>
    /// </summary>
    public string WorldName { get; set; } = string.Empty;
    
    /// <summary>
    /// <para>Seed</para>
    /// <para>种子</para>
    /// </summary>
    public int Seed { get; set; }
}