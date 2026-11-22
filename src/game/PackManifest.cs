using GlimmerWorks.Engine.Mods;
using Tomlyn.Serialization;

namespace GlimmerWorks.Game;

/// <summary>
/// Pack manifest, mirroring pack.toml.
/// 资源包清单，对应 pack.toml。
/// </summary>
public sealed class PackManifest
{
    [TomlPropertyName("id")] public string Id { get; init; } = string.Empty;

    /// <summary>
    /// Minimum game version this pack requires.
    /// 此资源包要求的最低游戏版本。
    /// </summary>
    [TomlPropertyName("min_game_code")]
    public long MinGameCode { get; init; }

    /// <summary>
    /// Whether this pack is a resource pack (no code/logic).
    /// 是否为资源包（不含代码/逻辑）。
    /// </summary>
    [TomlPropertyName("res_pack")]
    public bool ResPack { get; init; } = false;

    [TomlPropertyName("pack_dependencies")]
    public PackDependency[] PackDependencies { get; init; } = [];
}

/// <summary>
/// A dependency on another pack.
/// 对另一个资源包的依赖。
/// </summary>
public sealed class PackDependency
{
    [TomlPropertyName("min_version")] public long MinVersion { get; init; }

    [TomlPropertyName("pack_id")] public string PackId { get; init; } = string.Empty;
}