using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Graphics;

namespace GlimmerWorks.Game.Mods.assetsParser;

public abstract class ShaderAssetsParser : BaseAssetsParser<CompiledShader>
{
    public sealed override string GetFolderName()
    {
        return "shaders";
    }

    private static CompiledShader? Compile(bool vertexShader, GameAppContext appContext, FileEntry fileEntry)
    {
        var path = fileEntry.Path;
        if (path == null)
        {
            return null;
        }

        var fileName = fileEntry.Name;
        if (string.IsNullOrEmpty(fileName))
        {
            return null;
        }

        var bytes = appContext.VirtualFileSystem.ReadAllBytes(path);
        if (bytes == null)
        {
            return null;
        }

        return ShaderCompiler.Compile(vertexShader, fileName, bytes);
    }

    protected abstract bool IsVertexShader();

    public sealed override CompiledShader? Parser(GameAppContext appContext, FileEntry fileEntry,
        string requesterPackId)
    {
        return Compile(IsVertexShader(), appContext, fileEntry);
    }

    public sealed override bool EnableFileCache()
    {
        return true;
    }
}