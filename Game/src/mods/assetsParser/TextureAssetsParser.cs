using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Attributes;
using GlimmerWorks.Game.Graphics;
using JetBrains.Annotations;

namespace GlimmerWorks.Game.Mods.assetsParser;

/// <summary>
/// <para>TextureAssetsParser</para>
/// <para>纹理资源解析器（PNG）。</para>
/// </summary>
[UsedImplicitly]
[AssetsPaster("texture")]
public class TextureAssetsParser : BaseAssetsParser<TextureData>
{
    public override string GetFolderName()
    {
        return "textures";
    }

    public override string GetFileTypeName()
    {
        return "png";
    }

    public override TextureData? Parser(GameAppContext appContext, FileEntry fileEntry,
        string requesterPackId)
    {
        var path = fileEntry.Path;
        if (path == null)
        {
            return null;
        }

        var bytes = appContext.VirtualFileSystem.ReadAllBytes(path);
        if (bytes == null)
        {
            LogCat.Error($"Failed to read texture: {path.Path}");
            return null;
        }

        return SdlImageDecoder.DecodePng(bytes, fileEntry.Name ?? path.Path);
    }
}
