using GlimmerWorks.engine.extensions;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods.Data;
using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods;

public abstract class BasePack(VirtualPath packRootPath, PackManifest manifest)
{
    /// <summary>
    /// <para>resourceTypeDictionary</para>
    /// <para>资源类型转需要序列化的目标类型</para>
    /// </summary>
    private readonly Dictionary<string, Type> _resourceTypeDictionary = new();

    public Dictionary<string, Type> ResourceTypeDictionary => _resourceTypeDictionary;


    /// <summary>
    /// <para>RegisterResourceType</para>
    /// <para>注册资源类型</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="resourceTypeDictionary"></param>
    public virtual void RegisterResourceType(AppContext appContext, Dictionary<string, Type> resourceTypeDictionary)
    {
    }

    /// <summary>
    /// <para>Manually register resources from the file</para>
    /// <para>手动从文件注册资源</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="fileEntry"></param>
    /// <param name="resourceType"></param>
    /// <param name="onMessage"></param>
    /// <returns></returns>
    protected virtual List<Resources>? RegisterResourceManually(AppContext appContext, string resourceType,
        FileEntry fileEntry,
        Action<PackMessage> onMessage)
    {
        return null;
    }

    /// <summary>
    /// <para>LoadAllTemplate</para>
    /// <para>加载并注册全部模板</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="onMessage"></param>
    /// <returns></returns>
    public void LoadAllTemplate(AppContext appContext,
        Action<PackMessage> onMessage)
    {
        var templateResourceList = LoadTemplate(appContext, onMessage);
        if (templateResourceList == null)
        {
            return;
        }

        foreach (var templateResource in templateResourceList)
        {
            RegisterResource(onMessage, appContext, templateResource);
        }
    }

    /// <summary>
    /// <para>LoadTemplate</para>
    /// <para>加载模板</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="onMessage"></param>
    /// <returns></returns>
    protected virtual List<TemplateResources>? LoadTemplate(AppContext appContext,
        Action<PackMessage> onMessage)
    {
        return null;
    }

    /// <summary>
    /// <para>Load</para>
    /// <para>加载</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="onMessage"></param>
    /// <returns></returns>
    public bool Load(AppContext appContext, Action<PackMessage> onMessage)
    {
        var startLoadMessage = StartLoadMessage();
        if (startLoadMessage != null)
        {
            onMessage.Invoke(new PackMessage(appContext.Localizer[startLoadMessage, Manifest.Id].Value));
        }

        if (!ShouldScanFiles())
        {
            return true;
        }

        var resourceCount = 0;
        var fileEntryList = appContext.VirtualFileSystem.List(packRootPath, recurseSubdirectories: true);
        if (fileEntryList != null)
        {
            foreach (var fileEntry in fileEntryList)
            {
                if (fileEntry.IsDirectory)
                {
                    continue;
                }

                var resourceType = fileEntry.GetResourceType();
                if (resourceType == null)
                {
                    continue;
                }

                if (_resourceTypeDictionary.TryGetValue(resourceType, out var type))
                {
                    //For resources that have specified the conversion type, automatic registration is performed.
                    //对于指定了转换类型的资源，自动注册。
                    var typeObj = appContext.VirtualFileSystem.ReadToml(fileEntry.Path, type);
                    if (typeObj is not Resources resource)
                    {
                        continue;
                    }

                    if (RegisterResource(onMessage, appContext, resource))
                    {
                        resourceCount++;
                    }
                }
                else
                {
                    var resources =
                        RegisterResourceManually(appContext, resourceType, fileEntry, onMessage);
                    if (resources == null)
                    {
                        continue;
                    }

                    resourceCount += resources.Count(resource => RegisterResource(onMessage, appContext, resource));
                }
            }
        }

        return resourceCount > 0;
    }

    /// <summary>
    /// <para>RegisterResource</para>
    /// <para>注册资源</para>
    /// </summary>
    /// <param name="onMessage"></param>
    /// <param name="appContext"></param>
    /// <param name="resource"></param>
    /// <returns></returns>
    private bool RegisterResource(Action<PackMessage> onMessage, AppContext appContext,
        Resources resource)
    {
        resource.PackId = manifest.Id;
        if (resource is not TemplateResources && !resource.ResourceId.IsSnakeCase())
        {
            onMessage.Invoke(new PackMessage(
                appContext.Localizer["{0} must use snake_case (expected: {1}, actual: {2}).", "resource_id",
                    resource.ResourceId.ToSnakeCase(), resource.ResourceId].Value)
            {
                Path = resource.VirtualPath,
                Level = LogLevel.Error
            });
            return false;
        }

        onMessage.Invoke(new PackMessage(
            appContext.Localizer["Register resource{0}：{1}:{2}", resource.GetType().Name, resource.PackId,
                resource.ResourceId].Value));
        return appContext.DataRepository.Register(resource);
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