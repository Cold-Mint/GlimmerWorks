using Tomlyn.Serialization;

namespace GlimmerWorks.Game;

/// <summary>
/// Game configuration, corresponding to the content of config.toml.
/// 游戏配置，对应 config.toml 的内容。
/// </summary>
public sealed class GameConfig
{
    [TomlPropertyName("config_version")] public long ConfigVersion { get; init; }

    /// <summary>
    /// UI language code, e.g. "zh_CN". Empty means follow the system.
    /// 界面语言代码，如 "zh_CN"。为空表示跟随系统。
    /// </summary>
    [TomlPropertyName("language")]
    public string Language { get; init; } = string.Empty;

    [TomlPropertyName("mod")] public ModConfig Mod { get; init; } = new();

    [TomlPropertyName("window")] public WindowConfig Window { get; init; } = new();
}

public sealed class ModConfig
{
    /// <summary>
    /// Enabled data packs (ids).
    /// 已启用的数据包 id 集合。
    /// </summary>
    [TomlPropertyName("enabled_data_packs")]
    public HashSet<string> EnabledDataPacks { get; init; } = ["glimmerworks"];

    /// <summary>
    /// Enabled asset packs (ids).
    /// 已启用的资源包 id 集合。
    /// </summary>
    [TomlPropertyName("enabled_asset_packs")]
    public HashSet<string> EnabledAssetPacks { get; init; } = ["glimmerworks_res"];
}

/// <summary>
/// The [window] section in config.toml.
/// config.toml 中的 [window] 段落。
/// </summary>
public sealed class WindowConfig
{
    [TomlPropertyName("width")] public int Width { get; init; } = 1280;

    [TomlPropertyName("height")] public int Height { get; init; } = 720;

    [TomlPropertyName("fullscreen")] public bool Fullscreen { get; init; }

    [TomlPropertyName("vsync")] public bool Vsync { get; init; } = true;
}