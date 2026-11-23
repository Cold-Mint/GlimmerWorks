using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods.Data;
using System.Reflection;
using GlimmerWorks.Engine.Attributes;
using GlimmerWorks.Engine.Extensions;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Engine.Mods;

public class DataPack<TAppContext>(VirtualPath packRootPath, PackManifest manifest)
    : BasePack<TAppContext>(packRootPath, manifest) where TAppContext : AppContext
{
    private readonly VirtualPath _packRootPath = packRootPath;

    protected override bool ShouldScanFiles()
    {
        return true;
    }


    private static bool CheckPath(Action<PackMessage> onMessage, VirtualPath path,
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
                "Path segment {0} in template must use snake_case (expected: {1}, actual: {2}).", fragment,
                fragment.ToSnakeCase(), fragment)
            {
                Path = path,
                Level = LogLevel.Error
            });
            return false;
        }

        return true;
    }

    protected override List<TemplateResources>? LoadTemplate(TAppContext appContext, Action<PackMessage> onMessage)
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
            if (!CheckPath(onMessage, path, pathSpitArray, suffixLength))
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

    public override void RegisterResourceType(TAppContext appContext, Dictionary<string, Type> resourceTypeDictionary)
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

    protected override void RegisterDependencies(TAppContext appContext, Action<PackMessage> onMessage)
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
                    "Data pack {0} depends on {1}, but {1} is not loaded.", Manifest.Id,
                    dependencyPackId)
                {
                    Level = LogLevel.Warn
                });
            }
        }
    }

    protected override void StartLoadMessage(TAppContext appContext, Action<PackMessage> onMessage)
    {
        onMessage.Invoke(new PackMessage("Load Data Pack:{0}...", Manifest.Id));
    }
}