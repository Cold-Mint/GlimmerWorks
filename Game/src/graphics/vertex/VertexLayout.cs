using System.Collections.Frozen;
using GlimmerWorks.Engine.Log;
using JetBrains.Annotations;
using SDL3;
using Tomlyn.Serialization;

namespace GlimmerWorks.Game.Graphics.Vertex;

/// <summary>
/// <para>
/// A vertex layout asset: the stride and attributes that connect a vertex buffer
/// to the vertex shader inputs. Only the engine-supported semantics are allowed.
/// </para>
/// <para>
/// 顶点布局资产：连接顶点缓冲区与顶点着色器输入的 stride 和属性。
/// 只允许引擎支持的语义。
/// </para>
/// </summary>
public sealed class VertexLayout
{
    /// <summary>
    /// <para>The maximum supported vertex stride in bytes.</para>
    /// <para>支持的最大顶点步长（字节）。</para>
    /// </summary>
    public const int MaxStride = 32;

    private static readonly FrozenDictionary<string, VertexFormat> Formats =
        new Dictionary<string, VertexFormat>(StringComparer.Ordinal)
        {
            ["float"] = new(SDL.GPUVertexElementFormat.Float, 4),
            ["float2"] = new(SDL.GPUVertexElementFormat.Float2, 8),
            ["float3"] = new(SDL.GPUVertexElementFormat.Float3, 12),
            ["float4"] = new(SDL.GPUVertexElementFormat.Float4, 16),
        }.ToFrozenDictionary(StringComparer.Ordinal);

    /// <summary>
    /// <para>The built-in position + UV layout used by material pipelines.</para>
    /// <para>材质管线使用的内置“位置 + UV”布局。</para>
    /// </summary>
    public static VertexLayout PositionTexture { get; } = CreatePositionTexture();

    /// <summary>
    /// <para>The built-in position + UV + color layout used by the UI pipeline.</para>
    /// <para>UI 管线使用的内置“位置 + UV + 颜色”布局。</para>
    /// </summary>
    public static VertexLayout PositionTextureColor { get; } = CreatePositionTextureColor();

    /// <summary>
    /// <para>The layout name (informational).</para>
    /// <para>布局名称（仅用于描述）。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    /// <summary>
    /// <para>The byte stride of one vertex.</para>
    /// <para>单个顶点的字节步长。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("stride")]
    public int Stride { get; set; }

    /// <summary>
    /// <para>The attributes of the layout.</para>
    /// <para>布局的属性列表。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("attributes")]
    public List<VertexAttributeDefinition> Attributes { get; set; } = [];

    /// <summary>
    /// <para>Whether the layout provides a vertex color.</para>
    /// <para>布局是否提供顶点颜色。</para>
    /// </summary>
    [TomlIgnore]
    public bool HasColor => Attributes.Exists(attribute => attribute.Semantic == VertexSemantics.Color);

    /// <summary>
    /// <para>Check whether the layout provides the given semantic.</para>
    /// <para>检查布局是否提供指定的语义。</para>
    /// </summary>
    public bool HasSemantic(string semantic)
    {
        return Attributes.Exists(attribute => attribute.Semantic == semantic);
    }

    /// <summary>
    /// <para>Find the attribute providing the given semantic.</para>
    /// <para>查找提供指定语义的属性。</para>
    /// </summary>
    public VertexAttributeDefinition? FindAttribute(string semantic)
    {
        return Attributes.Find(attribute => attribute.Semantic == semantic);
    }

    /// <summary>
    /// <para>Validate the layout and resolve the element formats.</para>
    /// <para>校验布局并解析元素格式。</para>
    /// </summary>
    /// <returns>
    ///<para><c>true</c> when the layout is supported by the engine.</para>
    ///<para>布局被引擎支持时返回 <c>true</c>。</para>
    /// </returns>
    public bool PrepareLayout()
    {
        var valid = true;
        var name = string.IsNullOrEmpty(Name) ? "(unnamed)" : Name;

        if (Stride <= 0 || Stride > MaxStride || Stride % sizeof(float) != 0)
        {
            LogCat.Error(
                $"Vertex layout '{name}' has an invalid stride: {Stride} (expected a positive multiple of 4 up to {MaxStride}).");
            valid = false;
        }

        if (Attributes.Count == 0)
        {
            LogCat.Error($"Vertex layout '{name}' has no attributes.");
            return false;
        }

        var locations = new HashSet<uint>();
        var semantics = new HashSet<string>(StringComparer.Ordinal);
        var ranges = new List<(int Start, int End)>();

        foreach (var attribute in Attributes)
        {
            if (!locations.Add(attribute.Location))
            {
                LogCat.Error($"Vertex layout '{name}' has a duplicate location: {attribute.Location}.");
                valid = false;
            }

            if (string.IsNullOrEmpty(attribute.Semantic) || !semantics.Add(attribute.Semantic))
            {
                LogCat.Error(
                    $"Vertex layout '{name}' has an empty or duplicate semantic: '{attribute.Semantic}'.");
                valid = false;
            }

            if (!Formats.TryGetValue(attribute.Format, out var format))
            {
                LogCat.Error(
                    $"Vertex layout '{name}' attribute '{attribute.Semantic}' has an unsupported format: {attribute.Format}.");
                valid = false;
                continue;
            }

            attribute.VertexFormat = format;

            if (!ValidateSemanticFormat(name, attribute, format.Format))
            {
                valid = false;
            }

            if (attribute.Offset < 0 || attribute.Offset % sizeof(float) != 0)
            {
                LogCat.Error(
                    $"Vertex layout '{name}' attribute '{attribute.Semantic}' has an invalid offset: {attribute.Offset}.");
                valid = false;
            }

            var end = attribute.Offset + attribute.VertexFormat.Size;
            if (end > Stride)
            {
                LogCat.Error(
                    $"Vertex layout '{name}' attribute '{attribute.Semantic}' exceeds the stride: {end} > {Stride}.");
                valid = false;
            }

            ranges.Add((attribute.Offset, end));
        }

        ranges.Sort((left, right) => left.Start.CompareTo(right.Start));
        for (var i = 1; i < ranges.Count; i++)
        {
            if (ranges[i].Start < ranges[i - 1].End)
            {
                LogCat.Error($"Vertex layout '{name}' has overlapping attributes.");
                valid = false;
                break;
            }
        }

        if (!semantics.Contains(VertexSemantics.Position) || !semantics.Contains(VertexSemantics.Uv))
        {
            LogCat.Error($"Vertex layout '{name}' must provide both 'position' and 'uv'.");
            valid = false;
        }

        return valid;
    }

    private static bool ValidateSemanticFormat(string layoutName, VertexAttributeDefinition attribute,
        SDL.GPUVertexElementFormat format)
    {
        switch (attribute.Semantic)
        {
            case VertexSemantics.Position:
            case VertexSemantics.Uv:
                if (format != SDL.GPUVertexElementFormat.Float2)
                {
                    LogCat.Error(
                        $"Vertex layout '{layoutName}' attribute '{attribute.Semantic}' must use float2, got {attribute.Format}.");
                    return false;
                }

                return true;
            case VertexSemantics.Color:
                if (format != SDL.GPUVertexElementFormat.Float4)
                {
                    LogCat.Error(
                        $"Vertex layout '{layoutName}' attribute '{attribute.Semantic}' must use float4, got {attribute.Format}.");
                    return false;
                }

                return true;
            default:
                LogCat.Error(
                    $"Vertex layout '{layoutName}' has an unsupported semantic: '{attribute.Semantic}'.");
                return false;
        }
    }

    private static VertexLayout CreatePositionTexture()
    {
        var layout = new VertexLayout
        {
            Name = "position_texture",
            Stride = 16,
            Attributes =
            [
                new VertexAttributeDefinition
                {
                    Location = 0,
                    Semantic = VertexSemantics.Position,
                    Format = "float2",
                    Offset = 0
                },
                new VertexAttributeDefinition
                {
                    Location = 1,
                    Semantic = VertexSemantics.Uv,
                    Format = "float2",
                    Offset = 8
                }
            ]
        };
        layout.PrepareLayout();
        return layout;
    }

    private static VertexLayout CreatePositionTextureColor()
    {
        var layout = new VertexLayout
        {
            Name = "position_texture_color",
            Stride = 32,
            Attributes =
            [
                new VertexAttributeDefinition
                {
                    Location = 0,
                    Semantic = VertexSemantics.Position,
                    Format = "float2",
                    Offset = 0
                },
                new VertexAttributeDefinition
                {
                    Location = 1,
                    Semantic = VertexSemantics.Uv,
                    Format = "float2",
                    Offset = 8
                },
                new VertexAttributeDefinition
                {
                    Location = 2,
                    Semantic = VertexSemantics.Color,
                    Format = "float4",
                    Offset = 16
                }
            ]
        };
        layout.PrepareLayout();
        return layout;
    }
}