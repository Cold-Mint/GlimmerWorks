using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Game;

/// <summary>
/// Game configuration, corresponding to the content of config.toml.
/// 游戏配置，对应 config.toml 的内容。
/// </summary>
[UsedImplicitly]
public sealed class GameConfig
{
    [UsedImplicitly]
    [TomlPropertyName("config_version")]
    public long ConfigVersion { get; set; }

    /// <summary>
    /// UI language code, e.g. "zh_CN". Empty means follow the system.
    /// 界面语言代码，如 "zh_CN"。为空表示跟随系统。
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("language")]
    public string Language { get; set; } = string.Empty;

    [UsedImplicitly]
    [TomlPropertyName("links")]
    public List<Link> Links { get; set; } = [];

    [UsedImplicitly]
    [TomlPropertyName("window")]
    public WindowConfig Window { get; set; } = new();
}

[UsedImplicitly]
public sealed class Link
{
    [UsedImplicitly]
    [TomlPropertyName("url")]
    public string Url { get; set; } = string.Empty;

    [UsedImplicitly]
    [TomlPropertyName("title")]
    public string Title { get; set; } = string.Empty;
}

/// <summary>
/// The [window] section in config.toml.
/// config.toml 中的 [window] 段落。
/// </summary>
[UsedImplicitly]
public sealed class WindowConfig
{
    [UsedImplicitly]
    [TomlPropertyName("width")]
    public int Width { get; set; } = 1280;

    [UsedImplicitly]
    [TomlPropertyName("height")]
    public int Height { get; set; } = 720;

    [UsedImplicitly]
    [TomlPropertyName("fullscreen")]
    public bool Fullscreen { get; set; }

    [UsedImplicitly]
    [TomlPropertyName("vsync")]
    public bool Vsync { get; set; } = true;
}