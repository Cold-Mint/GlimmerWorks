using GlimmerWorks.Engine.Log;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine;

public class EngineConfig
{
    [UsedImplicitly]
    [TomlPropertyName("engine_config_version")]
    public long EngineConfigVersion { get; set; }


    [UsedImplicitly]
    [TomlPropertyName("data_pack_path")]
    public string DataPackPath { get; set; } = "mods/data_packs";


    [UsedImplicitly]
    [TomlPropertyName("cache_path")]
    public string CachePath { get; set; } = "runtime/.cache";

    [UsedImplicitly]
    [TomlPropertyName("log_level_mask")]
    public LogLevels LogLevelMask { set; get; } = LogLevels.Debug | LogLevels.Info | LogLevels.Warn | LogLevels.Error;
}