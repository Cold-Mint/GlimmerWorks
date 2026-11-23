using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods;

/// <summary>
/// Pack manifest, mirroring pack.toml.
/// 资源包清单，对应 pack.toml。
/// </summary>
[UsedImplicitly]
public sealed class PackManifest
{
    [UsedImplicitly]
    [TomlPropertyName("id")]
    public string Id { get; set; } = string.Empty;

    /// <summary>
    /// Minimum game version this pack requires.
    /// 此资源包要求的最低游戏版本。
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("min_game_code")]
    public long MinGameCode { get; set; }

    /// <summary>
    /// Whether this pack is a resource pack (no code/logic).
    /// 是否为资源包（不含代码/逻辑）。
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("res_pack")]
    public bool ResPack { get; set; }

    [UsedImplicitly]
    [TomlPropertyName("pack_dependencies")]
    public PackDependency[] PackDependencies { get; set; } = [];
}

/// <summary>
/// A dependency on another pack.
/// 对另一个资源包的依赖。
/// </summary>
[UsedImplicitly]
public sealed class PackDependency
{
    [UsedImplicitly]
    [TomlPropertyName("min_version")]
    public long MinVersion { get; set; }

    [UsedImplicitly]
    [TomlPropertyName("pack_id")]
    public string PackId { get; set; } = string.Empty;
}