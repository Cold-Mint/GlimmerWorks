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
}