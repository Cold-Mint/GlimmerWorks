using GlimmerWorks.engine.extensions;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods.Data;
using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;
using System.Reflection;
using GlimmerWorks.engine;
using GlimmerWorks.engine.attributes;

namespace GlimmerWorks.Engine.Mods;

public class DataPack(VirtualPath packRootPath, PackManifest manifest) : BasePack(packRootPath, manifest)
{
    private readonly VirtualPath _packRootPath = packRootPath;
    private int _stringMatchLevel;
    private readonly List<Resources> _stringResourcesList = new();

    protected override bool ShouldScanFiles()
    {
        return true;
    }


    private bool CheckPath(AppContext appContext, Action<PackMessage> onMessage, VirtualPath path,
        string[] pathSpitArray, int suffixLength)
    {
        for (var i = 0; i < pathSpitArray.Length; i++)
        {
            var fragment = pathSpitArray[i];
            if (i == pathSpitArray.Length - 1)
            {
                fragment = fragment[..^suffixLength];
            }

            if (fragment.IsSnakeCase())
            {
                continue;
            }

            onMessage.Invoke(new PackMessage(
                appContext.Localizer[
                    "Path segment {0} in template must use snake_case (expected: {1}, actual: {2}).", fragment,
                    fragment.ToSnakeCase(), fragment].Value)
            {
                Path = path,
                Level = LogLevel.Error
            });
            return false;
        }

        return true;
    }

    protected override List<TemplateResources>? LoadTemplate(AppContext appContext, Action<PackMessage> onMessage)
    {
        var templateEntryList =
            appContext.VirtualFileSystem.List(_packRootPath, "*.template.toml", recurseSubdirectories: true);
        if (templateEntryList == null)
        {
            return null;
        }

        List<TemplateResources> templateResourcesList = [];
        var suffixLength = ".template.toml".Length;
        foreach (var path in templateEntryList.Select(fileEntry => fileEntry.Path))
        {
            if (path == null)
            {
                continue;
            }

            var relativePath = path.GetRelativePath(_packRootPath);
            var pathSpitArray = relativePath.Split(
                [Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar],
                StringSplitOptions.None);
            if (!CheckPath(appContext, onMessage, path, pathSpitArray, suffixLength))
            {
                continue;
            }

            var text = appContext.VirtualFileSystem.ReadAllText(path);
            if (text == null)
            {
                continue;
            }

            templateResourcesList.Add(new TemplateResources
            {
                VirtualPath = path,
                ResourceId = relativePath.Substring(0, relativePath.Length - suffixLength).Replace('\\', '/'),
                Content = text
            });
        }

        return templateResourcesList;
    }

    private void RegisterStrings(AppContext appContext,
        FileEntry fileEntry, VirtualPath path)
    {
        var candidates = LocalizationManager.GetCandidates(appContext.LocalizationManager.CurrentLanguage);
        var currentFileName = fileEntry.Name?.ToLowerInvariant();
        if (_stringMatchLevel == 0 && currentFileName == "default.strings.toml")
        {
            //Default match.
            //默认匹配。
            var stringFileResources =
                appContext.VirtualFileSystem.ReadToml<StringSetResources>(path, new PackContext
                {
                    AppContext = appContext,
                    RequesterPackId = Manifest.Id
                });
            if (stringFileResources == null)
            {
                return;
            }

            _stringMatchLevel = 1;
            _stringResourcesList.AddRange(stringFileResources.StringList);
            _stringResourcesList.AddRange(stringFileResources.StringTagList);
        }

        foreach (var candidate in candidates)
        {
            var targetFileName = (candidate + ".strings.toml").ToLowerInvariant();
            if (currentFileName == targetFileName)
            {
                //Best match.
                //最佳匹配。
                var stringFileResources =
                    appContext.VirtualFileSystem.ReadToml<StringSetResources>(path, new PackContext
                    {
                        AppContext = appContext,
                        RequesterPackId = Manifest.Id
                    });
                if (stringFileResources == null)
                {
                    return;
                }

                _stringMatchLevel = 2;
                _stringResourcesList.Clear();
                _stringResourcesList.AddRange(stringFileResources.StringList);
                _stringResourcesList.AddRange(stringFileResources.StringTagList);
            }
        }
    }

    protected override List<Resources>? RegisterResourceManually(AppContext appContext, string resourceType,
        FileEntry fileEntry, Action<PackMessage> onMessage)
    {
        var path = fileEntry.Path;
        if (path == null)
        {
            return null;
        }

        if (resourceType == "strings" && _stringMatchLevel < 2)
        {
            RegisterStrings(appContext, fileEntry, path);
        }

        return null;
    }


    protected override List<Resources>? AfterScanFiles()
    {
        return _stringResourcesList;
    }


    public override void RegisterResourceType(AppContext appContext, Dictionary<string, Type> resourceTypeDictionary)
    {
        base.RegisterResourceType(appContext, resourceTypeDictionary);
        var resourceType = typeof(Resources);
        foreach (var type in resourceType.Assembly.GetTypes())
        {
            if (type.IsAbstract || !resourceType.IsAssignableFrom(type))
            {
                continue;
            }

            var attr = type.GetCustomAttribute<DataPackResourceAttribute>();
            if (attr is null)
            {
                continue;
            }

            if (!attr.AutoDeserialization)
            {
                continue;
            }

            resourceTypeDictionary.Add(attr.ResourceType, type);
        }
    }

    protected override void RegisterDependencies(AppContext appContext, Action<PackMessage> onMessage)
    {
        var dependencies = Manifest.PackDependencies;
        appContext.DataRepository.SetDependencies(Manifest.Id,
            dependencies.Select(static dependency => dependency.PackId));

        foreach (var dependencyPackId in dependencies.Select(dependency => dependency.PackId))
        {
            if (string.IsNullOrWhiteSpace(dependencyPackId) || dependencyPackId == "@self")
            {
                continue;
            }

            if (appContext.DataPackManager.GetPack(dependencyPackId) == null)
            {
                onMessage.Invoke(new PackMessage(
                    appContext.Localizer["Data pack {0} depends on {1}, but {1} is not loaded.", Manifest.Id,
                        dependencyPackId].Value)
                {
                    Level = LogLevel.Warn
                });
            }
        }
    }

    protected override void StartLoadMessage(AppContext appContext, Action<PackMessage> onMessage)
    {
        base.StartLoadMessage(appContext, onMessage);
        onMessage.Invoke(new PackMessage(appContext.Localizer["Load Data Pack:{0}...", Manifest.Id].Value));
    }
}