namespace GlimmerWorks.Game.Mods.assetsParser;

public class FragmentShaderAssetsParser : ShaderAssetsParser
{
    public override string GetResourceType()
    {
        return "fragShader";
    }

    public override string GetFileTypeName()
    {
        return "frag";
    }
}