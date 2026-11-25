using GlimmerWorks.Engine.Extensions;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods.Data;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Engine.Mods;

public abstract class BasePack<TAppContext>(VirtualPath packRootPath, PackManifest manifest)
    where TAppContext : AppContext
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
    public virtual void RegisterResourceType(TAppContext appContext, Dictionary<string, Type> resourceTypeDictionary)
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
    protected virtual List<Resources>? RegisterResourceManually(TAppContext appContext, string resourceType,
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
    public void LoadAllTemplate(TAppContext appContext,
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
    protected virtual List<TemplateResources>? LoadTemplate(TAppContext appContext,
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
    public bool Load(TAppContext appContext, Action<PackMessage> onMessage)
    {
        StartLoadMessage(appContext, onMessage);
        RegisterDependencies(appContext, onMessage);
        if (!ShouldScanFiles())
        {
            return true;
        }

        var fileEntryList = appContext.VirtualFileSystem.List(packRootPath, recurseSubdirectories: true);
        if (fileEntryList == null)
        {
            return false;
        }

        var resourceCount = 0;
        foreach (var fileEntry in fileEntryList)
        {
            if (fileEntry.IsDirectory)
            {
                continue;
            }

            resourceCount += HandleSingleFile(appContext, onMessage, fileEntry);
        }

        var afterScanFiles = AfterScanFiles();
        if (afterScanFiles != null)
        {
            resourceCount += afterScanFiles.Count(resource => RegisterResource(onMessage, appContext, resource));
        }

        return resourceCount > 0;
    }

    private int HandleSingleFile(TAppContext appContext, Action<PackMessage> onMessage, FileEntry fileEntry)
    {
        var resourceType = fileEntry.GetResourceType();
        if (resourceType == null)
        {
            return 0;
        }

        if (_resourceTypeDictionary.TryGetValue(resourceType, out var type))
        {
            //For resources that have specified the conversion type, automatic registration is performed.
            //对于指定了转换类型的资源，自动注册。
            var path = fileEntry.Path;
            if (path == null)
            {
                return 0;
            }

            var typeObj = appContext.VirtualFileSystem.ReadToml(path, type, new PackContext
            {
                AppContext = appContext,
                RequesterPackId = manifest.Id
            });
            if (typeObj is not Resources resource)
            {
                return 0;
            }

            if (RegisterResource(onMessage, appContext, resource))
            {
                return 1;
            }
        }
        else
        {
            var resources =
                RegisterResourceManually(appContext, resourceType, fileEntry, onMessage);
            if (resources == null)
            {
                return 0;
            }

            return resources.Count(resource => RegisterResource(onMessage, appContext, resource));
        }

        return 0;
    }

    /// <summary>
    /// <para>AfterScanFiles</para>
    /// <para>扫描文件之后</para>
    /// </summary>
    protected virtual List<Resources>? AfterScanFiles()
    {
        return null;
    }

    /// <summary>
    /// <para>RegisterResource</para>
    /// <para>注册资源</para>
    /// </summary>
    /// <param name="onMessage"></param>
    /// <param name="appContext"></param>
    /// <param name="resource"></param>
    /// <returns></returns>
    private bool RegisterResource(Action<PackMessage> onMessage, TAppContext appContext,
        Resources resource)
    {
        resource.PackId = manifest.Id;
        resource.DisplayName.SetPackId(manifest.Id);
        if (resource is not TemplateResources && !resource.ResourceId.IsSnakeCase())
        {
            onMessage.Invoke(new PackMessage(
                "{0} must use snake_case (expected: {1}, actual: {2}).", "resource_id",
                resource.ResourceId.ToSnakeCase(), resource.ResourceId)
            {
                Path = resource.VirtualPath,
                Level = LogLevels.Error
            });
            return false;
        }

        onMessage.Invoke(new PackMessage(
            "Register resource{0}：{1}:{2}", resource.GetType().Name, resource.PackId,
            resource.ResourceId));
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
    protected virtual void StartLoadMessage(TAppContext appContext, Action<PackMessage> onMessage)
    {
    }

    /// <summary>
    /// <para>RegisterDependencies</para>
    /// <para>注册清单中声明的包依赖</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="onMessage"></param>
    protected virtual void RegisterDependencies(TAppContext appContext, Action<PackMessage> onMessage)
    {
    }

    public PackManifest Manifest => manifest;
}