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


    [TomlPropertyName("window")] public WindowConfig Window { get; init; } = new();
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