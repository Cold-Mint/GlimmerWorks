using GlimmerWorks.Engine;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Attributes;
using GlimmerWorks.Game.Graphics.Vertex;
using JetBrains.Annotations;

namespace GlimmerWorks.Game.Mods.assetsParser;

/// <summary>
/// <para>VertexLayoutAssetsParser</para>
/// <para>顶点布局资源解析器。</para>
/// </summary>
[UsedImplicitly]
[AssetsPaster("vertex_layout")]
public class VertexLayoutAssetsParser : BaseAssetsParser<VertexLayout>
{
    public override string GetFolderName()
    {
        return "pipelines";
    }

    public override string GetFileTypeName()
    {
        return "vertex_layout.toml";
    }

    public override VertexLayout? Parser(GameAppContext appContext, FileEntry fileEntry,
        string requesterPackId)
    {
        var path = fileEntry.Path;
        if (path == null)
        {
            return null;
        }

        var layout = appContext.VirtualFileSystem.ReadToml<VertexLayout>(path, new PackContext
        {
            AppContext = appContext,
            RequesterPackId = requesterPackId
        });
        if (layout == null)
        {
            LogCat.Error($"Failed to read vertex layout: {path.Path}");
            return null;
        }

        if (!layout.PrepareLayout())
        {
            LogCat.Error($"Vertex layout '{path.Path}' is invalid.");
            return null;
        }

        return layout;
    }
}
