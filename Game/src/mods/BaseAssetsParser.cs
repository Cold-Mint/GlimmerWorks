namespace GlimmerWorks.Game.Mods;

/// <summary>
/// <para>IAssetsParser</para>
/// <para>资产解析器</para>
/// </summary>
public abstract class BaseAssetsParser
{
    /// <summary>
    /// <para>GetFolderName</para>
    /// <para>获取资产文件夹名，例如：所有的纹理都放在textures文件夹下，那么此方法返回textures。</para>
    /// </summary>
    /// <returns></returns>
    public abstract string GetFolderName();

    /// <summary>
    /// <para>GetFileTypeName</para>
    /// <para>文件类型名</para>
    /// </summary>
    /// <returns></returns>
    public abstract string GetFileTypeName();

    /// <summary>
    /// <para>Parser</para>
    /// <para>解析资产</para>
    /// </summary>
    /// <param name="appContext">
    ///<para>appContext</para>
    ///<para>应用上下文</para>
    /// </param>
    /// <param name="bytes">
    ///<para>bytes</para>
    ///<para>文件字节</para>
    /// </param>
    /// <returns></returns>
    public abstract T? Parser<T>(GameAppContext appContext, byte[] bytes);


    /// <summary>
    /// <para>GetResourceType</para>
    /// <para>获取资源类型</para>
    /// </summary>
    /// <returns></returns>
    public virtual string GetResourceType()
    {
        return GetFolderName();
    }
}