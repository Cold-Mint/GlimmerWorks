namespace GlimmerWorks.Game.Mods;

public class AssetResult<T>
{
    /// <summary>
    /// <para>Exist</para>
    /// <para>资源是存在</para>
    /// </summary>
    public bool Exist { get; set; } = false;

    /// <summary>
    /// <para>Resource results</para>
    /// <para>资源结果</para>
    /// </summary>
    public T? Result { get; set; }

    /// <summary>
    /// <para>If it exists, from which resource pack was it found?</para>
    /// <para>如果存在的话，从哪个资源包找到的</para>
    /// </summary>
    public AssetsPack? AssetsPack { get; set; } = null;
}