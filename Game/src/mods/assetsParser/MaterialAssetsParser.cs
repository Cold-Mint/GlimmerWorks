using GlimmerWorks.Engine;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Attributes;
using GlimmerWorks.Game.Graphics;
using JetBrains.Annotations;

namespace GlimmerWorks.Game.Mods.assetsParser;

/// <summary>
/// <para>MaterialAssetsParser</para>
/// <para>材质资源解析器。</para>
/// </summary>
[UsedImplicitly]
[AssetsPaster("material")]
public class MaterialAssetsParser : BaseAssetsParser<Material>
{
    public override string GetFolderName()
    {
        return "materials";
    }

    public override string GetFileTypeName()
    {
        return "material.toml";
    }

    public override Material? Parser(GameAppContext appContext, FileEntry fileEntry, string requesterPackId)
    {
        var path = fileEntry.Path;
        if (path == null)
        {
            return null;
        }
        var definition = appContext.VirtualFileSystem.ReadToml<MaterialDefinition>(path, new PackContext
        {
            AppContext = appContext,
            RequesterPackId = requesterPackId
        });
        if (definition == null)
        {
            LogCat.Error($"Failed to read material: {path.Path}");
            return null;
        }

        var texture = Resolve<TextureData>(appContext, definition.Texture, ResourceType.Texture,
            requesterPackId);
        if (texture == null)
        {
            LogCat.Error($"Material '{path.Path}' has no valid texture.");
            return null;
        }

        var pipeline = Resolve<Pipeline>(appContext, definition.Pipeline, ResourceType.Pipeline,
            requesterPackId);
        if (pipeline == null)
        {
            LogCat.Error($"Material '{path.Path}' has no valid pipeline.");
            return null;
        }

        return new Material
        {
            Texture = texture,
            Pipeline = pipeline
        };
    }


    private static T? Resolve<T>(GameAppContext appContext, ResourceRef? reference, string defaultResourceType,
        string requesterPackId)
    {
        if (reference == null || string.IsNullOrWhiteSpace(reference.ResourceId))
        {
            return default;
        }

        reference.SetPackId(requesterPackId);
        if (string.IsNullOrWhiteSpace(reference.ResourceType))
        {
            reference.ResourceType = defaultResourceType;
        }

        var result = appContext.AssetsPackManager.FindAsset<T>(appContext, reference);
        return result.Exist ? result.Result : default;
    }
}