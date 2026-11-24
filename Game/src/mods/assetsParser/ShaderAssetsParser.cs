namespace GlimmerWorks.Game.Mods.assetsParser;

public abstract class ShaderAssetsParser : BaseAssetsParser
{
    public override string GetFolderName()
    {
        return "shaders";
    }

    public override T? Parser<T>(GameAppContext appContext, byte[]? bytes) where T : default
    {
        throw new NotImplementedException();
    }
}