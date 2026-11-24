namespace GlimmerWorks.Game.Mods.assetsParser;

/// <summary>
/// <para>VertexShaderAssetsParser</para>
/// <para>顶点着色器资源解析器</para>
/// </summary>
public class VertexShaderAssetsParser : ShaderAssetsParser
{
    public override string GetResourceType()
    {
        return "vertShader";
    }


    public override string GetFileTypeName()
    {
        return "vert";
    }
}