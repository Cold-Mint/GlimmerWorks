using GlimmerWorks.engine.extensions;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods.Data;
using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;
using System.Reflection;
using GlimmerWorks.engine;

namespace GlimmerWorks.Engine.Mods;

public class DataPack(VirtualPath packRootPath, PackManifest manifest) : BasePack(packRootPath, manifest)
{
    private readonly VirtualPath _packRootPath = packRootPath;

    protected override bool ShouldScanFiles()
    {
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
        foreach (var fileEntry in templateEntryList)
        {
            var path = fileEntry.Path.Path;
            var pathSpitArray = path.Split(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
            var allSnakeCase = true;
            for (var i = 0; i < pathSpitArray.Length; i++)
            {
                var fragment = pathSpitArray[i];
                if (i == pathSpitArray.Length - 1)
                {
                    fragment = fragment[..^suffixLength];
                }

                if (!fragment.IsSnakeCase())
                {
                    allSnakeCase = false;
                    onMessage.Invoke(new PackMessage(
                        appContext.Localizer[
                            "Path segment {0} in template must use snake_case (expected: {1}, actual: {2}).", fragment,
                            fragment.ToSnakeCase(), fragment].Value)
                    {
                        Path = fileEntry.Path,
                        Level = LogLevel.Error
                    });
                    break;
                }
            }

            if (!allSnakeCase)
            {
                continue;
            }

            var text = appContext.VirtualFileSystem.ReadAllText(fileEntry.Path);
            if (text == null)
            {
                continue;
            }

            var relativePath = fileEntry.Path.GetRelativePath(_packRootPath);
            templateResourcesList.Add(new TemplateResources
            {
                VirtualPath = fileEntry.Path,
                ResourceId = relativePath.Substring(0, relativePath.Length - suffixLength),
                Content = text
            });
        }

        return templateResourcesList;
    }

    protected override List<Resources>? RegisterResourceManually(AppContext appContext, string resourceType,
        FileEntry fileEntry, Action<PackMessage> onMessage)
    {
        if (resourceType == "strings")
        {
            var stringFileResources =
                appContext.VirtualFileSystem.ReadToml<StringSetResources>(fileEntry.Path);
            if (stringFileResources == null)
            {
                return null;
            }

            return [.. stringFileResources.StringList, .. stringFileResources.StringTagList];
        }

        return null;
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

            var attr = type.GetCustomAttribute<DataPackResource>();
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

    protected override void StartLoadMessage(AppContext appContext, Action<PackMessage> onMessage)
    {
        base.StartLoadMessage(appContext, onMessage);
        onMessage.Invoke(new PackMessage(appContext.Localizer["Load Data Pack:{0}...", Manifest.Id].Value));
    }
}