using GlimmerWorks.Engine;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Attributes;
using GlimmerWorks.Game.Graphics.Uniform;
using JetBrains.Annotations;

namespace GlimmerWorks.Game.Mods.assetsParser;

/// <summary>
/// <para>UniformBlockAssetsParser</para>
/// <para>Uniform Block 资源解析器。</para>
/// </summary>
[UsedImplicitly]
[AssetsPaster("uniform_block")]
public class UniformBlockAssetsParser : BaseAssetsParser<UniformBlock>
{
    public override string GetFolderName()
    {
        return "shaders";
    }

    public override string GetFileTypeName()
    {
        return "uniforms.toml";
    }

    public override UniformBlock? Parser(GameAppContext appContext, FileEntry fileEntry, string requesterPackId)
    {
        var path = fileEntry.Path;
        if (path == null)
        {
            return null;
        }

        var block = appContext.VirtualFileSystem.ReadToml<UniformBlock>(path, new PackContext
        {
            AppContext = appContext,
            RequesterPackId = requesterPackId
        });
        if (block == null)
        {
            LogCat.Error($"Failed to read uniform block: {path.Path}");
            return null;
        }

        if (!block.PrepareLayout())
        {
            LogCat.Error($"Uniform block '{block.Name}' contains unsupported member types.");
            return null;
        }

        return block;
    }
}
