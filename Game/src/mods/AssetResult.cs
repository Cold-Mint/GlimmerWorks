namespace GlimmerWorks.Game.Mods;

public class AssetResult<T>
{
    /// <summary>
    /// <para>Exist</para>
    /// <para>资源是存在</para>
    /// </summary>
    public bool Exist => !Equals(Result, default(T));

    /// <summary>
    /// <para>Resource results</para>
    /// <para>资源结果</para>
    /// </summary>
    public T? Result { get; set; }
}