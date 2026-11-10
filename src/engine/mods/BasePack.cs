using GlimmerWorks.Engine.Mods.Data;
using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods;

public abstract class BasePack(PackManifest manifest)
{
    /// <summary>
    /// <para>resourceTypeDictionary</para>
    /// <para>资源类型转需要序列化的目标类型</para>
    /// </summary>
    private readonly Dictionary<string, Type?> _resourceTypeDictionary = new();

    public Dictionary<string, Type> ResourceTypeDictionary => _resourceTypeDictionary;


    /// <summary>
    /// <para>GetResourceType</para>
    /// <para>获取文件的资源类型</para>
    /// </summary>
    /// <param name="fileName"></param>
    /// <returns></returns>
    private static string? GetResourceType(string fileName)
    {
        var span = Path.GetFileName(fileName.AsSpan());
        var lastDot = span.LastIndexOf('.');
        if (lastDot <= 0)
        {
            return null;
        }

        span = span[..lastDot];
        var prevDot = span.LastIndexOf('.');
        return prevDot < 0 ? null : span[(prevDot + 1)..].ToString();
    }

    /// <summary>
    /// <para>RegisterResourceType</para>
    /// <para>注册资源类型</para>
    /// </summary>
    /// <param name="resourceTypeDictionary"></param>
    protected virtual void RegisterResourceType(Dictionary<string, Type> resourceTypeDictionary)
    {
    }


    /// <summary>
    /// <para>Load</para>
    /// <para>加载</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="virtualPath"></param>
    /// <param name="onMessage"></param>
    /// <returns></returns>
    public bool Load(AppContext appContext, VirtualPath virtualPath, Action<PackMessage> onMessage)
    {
        var startLoadMessage = StartLoadMessage();
        if (startLoadMessage != null)
        {
            onMessage.Invoke(new PackMessage(appContext.LocalizationManager.GetFormat(startLoadMessage, Manifest.Id)));
        }

        if (!ShouldScanFiles())
        {
            return true;
        }

        var fileEntryList = appContext.VirtualFileSystem.List(virtualPath, recurseSubdirectories: true);
        if (fileEntryList == null)
        {
            return false;
        }

        foreach (var fileEntry in fileEntryList)
        {
            if (fileEntry.IsDirectory)
            {
                continue;
            }

            var resourceType = GetResourceType(fileEntry.Name);
            if (resourceType == null)
            {
                continue;
            }

            if (_resourceTypeDictionary.TryGetValue(resourceType, out Type type))
            {
                var typeObj = appContext.VirtualFileSystem.ReadToml(fileEntry.Path, type);
                if (typeObj is not Resource resource)
                {
                    continue;
                }

                if (!appContext.DataRepository.Register(resource))
                {
                    continue;
                }
            }
        }

        return false;
    }

    /// <summary>
    /// <para>ShouldScanFiles</para>
    /// <para>是否需要扫描文件</para>
    /// </summary>
    /// <returns></returns>
    protected abstract bool ShouldScanFiles();

    /// <summary>
    /// <para>StartLoadMessage</para>
    /// <para>开始加载时的消息</para>
    /// </summary>
    /// <returns></returns>
    protected virtual string? StartLoadMessage()
    {
        return null;
    }

    public PackManifest Manifest => manifest;
}