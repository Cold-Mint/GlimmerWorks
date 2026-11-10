using GlimmerWorks.Engine.Mods;
using Tomlyn.Serialization;

namespace GlimmerWorks.Game;

/// <summary>
/// Pack manifest, mirroring pack.toml.
/// 资源包清单，对应 pack.toml。
/// </summary>
public sealed class PackManifest
{
    [TomlPropertyName("author")]
    public string Author { get; init; } = string.Empty;

    [TomlPropertyName("id")]
    public string Id { get; init; } = string.Empty;

    /// <summary>
    /// Minimum game version this pack requires.
    /// 此资源包要求的最低游戏版本。
    /// </summary>
    [TomlPropertyName("min_game_version")]
    public long MinGameVersion { get; init; }

    /// <summary>
    /// Whether this pack is a resource pack (no code/logic).
    /// 是否为资源包（不含代码/逻辑）。
    /// </summary>
    [TomlPropertyName("res_pack")]
    public bool ResPack { get; init; }

    [TomlPropertyName("version_name")]
    public string VersionName { get; init; } = string.Empty;

    [TomlPropertyName("version_number")]
    public long VersionNumber { get; init; }

    /// <summary>
    /// Folders searched for templates (recursively). "@root" means the pack root.
    /// 模板搜索目录（递归）。"@root" 表示资源包根目录。
    /// </summary>
    [TomlPropertyName("template_search_path")]
    public string[] TemplateSearchPath { get; init; } = [];

    [TomlPropertyName("description")]
    public ResourceRef Description { get; init; } = new();

    [TomlPropertyName("name")]
    public ResourceRef Name { get; init; } = new();

    [TomlPropertyName("pack_dependencies")]
    public PackDependency[] PackDependencies { get; init; } = [];
}

/// <summary>
/// A dependency on another pack.
/// 对另一个资源包的依赖。
/// </summary>
public sealed class PackDependency
{
    [TomlPropertyName("min_version")]
    public long MinVersion { get; init; }

    [TomlPropertyName("pack_id")]
    public string PackId { get; init; } = string.Empty;
}