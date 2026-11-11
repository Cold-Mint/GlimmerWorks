using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods.PackLoader;

/// <summary>
/// <para>DataPackLoader</para>
/// <para>数据包加载器</para>
/// </summary>
public class DataPackLoader : PackLoader<DataPack>
{
    protected override HashSet<string> GetEnabledSet(ModConfig modConfig)
    {
        return modConfig.EnabledDataPacks;
    }

    protected override DataPack CreatePack(VirtualPath packRootPath, PackManifest manifest)
    {
        return new DataPack(packRootPath, manifest);
    }
    
}