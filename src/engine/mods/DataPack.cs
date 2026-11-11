using GlimmerWorks.Engine.Mods.Data;
using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

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
        foreach (var fileEntry in templateEntryList)
        {
            var text = appContext.VirtualFileSystem.ReadAllText(fileEntry.Path);
            if (text == null)
            {
                continue;
            }

            templateResourcesList.Add(new TemplateResources
            {
                ResourceId = fileEntry.Path.GetRelativePath(_packRootPath),
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
                appContext.VirtualFileSystem.ReadToml<StringFileResources>(fileEntry.Path);
            if (stringFileResources == null)
            {
                return null;
            }

            return [.. stringFileResources.StringList, .. stringFileResources.StringTagList];
        }

        return null;
    }

    public override void RegisterResourceType(Dictionary<string, Type> resourceTypeDictionary)
    {
        base.RegisterResourceType(resourceTypeDictionary);
        resourceTypeDictionary.Add("dimension", typeof(DimensionResources));
    }

    protected override string StartLoadMessage()
    {
        return "ui.load_data_pack";
    }
}