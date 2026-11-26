using GlimmerWorks.Game.Attributes;
using JetBrains.Annotations;

namespace GlimmerWorks.Game.Mods.assetsParser;

/// <summary>
/// <para>VertexShaderAssetsParser</para>
/// <para>顶点着色器资源解析器</para>
/// </summary>
[UsedImplicitly]
[AssetsPaster("shader_vert")]
public class VertexShaderAssetsParser : ShaderAssetsParser
{
    public override string GetFileTypeName()
    {
        return "vert";
    }

    protected override bool IsVertexShader()
    {
        return true;
    }
}