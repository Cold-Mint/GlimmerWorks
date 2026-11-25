using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Game.Mods;

/// <summary>
/// <para>IAssetsParser</para>
/// <para>资产解析器</para>
/// </summary>
public abstract class BaseAssetsParser<T>
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
    /// <param name="fileEntry">
    ///<para>fileEntry</para>
    ///<para>文件实体</para>
    /// </param>
    /// <returns></returns>
    public abstract T? Parser(GameAppContext appContext, FileEntry fileEntry);


    /// <summary>
    /// <para>Should file caching be enabled? Use the method provided by the engine to determine if the file has changed, thereby utilizing the file cache. For example, after compiling the shader, a file cache for the shader will be established.</para>
    /// <para>是否启用文件缓存，使用引擎提供的方法判断文件是否改变，从而利用文件缓存。例如编译着色器后会对着色器建立文件缓存。</para>
    /// </summary>
    /// <returns></returns>
    public virtual bool EnableFileCache()
    {
        return false;
    }
}