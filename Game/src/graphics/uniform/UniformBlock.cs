using GlimmerWorks.Engine.Log;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Game.Graphics.Uniform;

/// <summary>
/// <para>
/// A uniform block asset: a block name plus its members. Members are either
/// runtime-injected (source starts with <c>@runtime.</c>) or static values.
/// </para>
/// <para>
/// Uniform 块资产：块名与成员列表。成员可以是运行时注入
/// （source 以 <c>@runtime.</c> 开头），也可以是静态值。
/// </para>
/// </summary>
public sealed class UniformBlock
{
    /// <summary>
    /// <para>The prefix that marks a runtime-injected member source.</para>
    /// <para>标记运行时注入成员 source 的前缀。</para>
    /// </summary>
    private const string RuntimeSourcePrefix = "@runtime.";

    /// <summary>
    /// <para>The block name. It must match the block name in the shader.</para>
    /// <para>Uniform 块名，需与着色器内 block 名一致。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    /// <summary>
    /// <para>The members of the block.</para>
    /// <para>块的成员。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("members")]
    public List<UniformMember> Members { get; set; } = [];

    /// <summary>
    /// <para>The packed block size in bytes (std140 layout, rounded up to 16 bytes).</para>
    /// <para>打包后的块大小（std140 布局，向上对齐到 16 字节）。</para>
    /// </summary>
    public int Size { get; private set; }

    /// <summary>
    /// <para>Compute member offsets from their declared types.</para>
    /// <para>根据声明的类型计算成员偏移。</para>
    /// </summary>
    /// <returns>
    ///<para><c>true</c> when every member type is supported.</para>
    ///<para>所有成员类型都受支持时返回 <c>true</c>。</para>
    /// </returns>
    public bool PrepareLayout()
    {
        var offset = 0;
        var valid = true;
        foreach (var member in Members)
        {
            if (UniformTypeInfo.TryParse(member.Type) is not { } info)
            {
                LogCat.Error($"Uniform block '{Name}' has an unsupported member type: {member.Type} ({member.Name})");
                valid = false;
                continue;
            }

            // std140: members are aligned to their base alignment.
            // std140：成员按其基础对齐值对齐。
            offset = Align(offset, info.Alignment);
            member.Offset = offset;
            member.TypeInfo = info;
            offset += info.Size;

            if (!string.IsNullOrEmpty(member.Source) &&
                !member.Source.StartsWith(RuntimeSourcePrefix, StringComparison.Ordinal) &&
                member.Value is not { Count: > 0 })
            {
                LogCat.Warn(
                    $"Uniform block '{Name}' member '{member.Name}' has an unknown source: {member.Source}");
            }
        }

        Size = Align(offset, 16);
        return valid;
    }

    /// <summary>
    /// <para>Build the packed block data for the current frame.</para>
    /// <para>构建当前帧的打包块数据。</para>
    /// </summary>
    /// <param name="runtimeUniformManager">
    ///<para>The runtime uniform value source.</para>
    ///<para>运行时 uniform 值来源。</para>
    /// </param>
    /// <returns></returns>
    public byte[] BuildData(RuntimeUniformManager runtimeUniformManager)
    {
        var data = new byte[Size];
        foreach (var member in Members)
        {
            if (member.TypeInfo.ComponentCount == 0)
            {
                continue;
            }

            if (member.Source.StartsWith(RuntimeSourcePrefix, StringComparison.Ordinal))
            {
                var key = member.Source[RuntimeSourcePrefix.Length..];
                if (runtimeUniformManager.TryGetValue(key, out var value))
                {
                    value.WriteTo(data, member.Offset, member.TypeInfo);
                }

                continue;
            }

            if (member.Value is { Count: > 0 })
            {
                UniformValue.FromNumbers(member.Value).WriteTo(data, member.Offset, member.TypeInfo);
            }
        }

        return data;
    }

    private static int Align(int value, int alignment)
    {
        return (value + alignment - 1) / alignment * alignment;
    }
}