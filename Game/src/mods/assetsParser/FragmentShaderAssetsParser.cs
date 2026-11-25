using GlimmerWorks.Game.Attributes;
using JetBrains.Annotations;

namespace GlimmerWorks.Game.Mods.assetsParser;

[UsedImplicitly]
[AssetsPaster("fragShader")]
public class FragmentShaderAssetsParser : ShaderAssetsParser
{
    public override string GetFileTypeName()
    {
        return "frag";
    }

    protected override bool IsVertexShader()
    {
        return false;
    }
}