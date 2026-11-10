using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods.PackLoader;

/// <summary>
/// <para>AssetsPackLoader</para>
/// <para>资产包加载器</para>
/// </summary>
public class AssetsPackLoader : PackLoader<AssetsPack>
{
    protected override HashSet<string> GetEnabledSet(ModConfig modConfig)
    {
        return modConfig.EnabledAssetPacks;
    }

    protected override AssetsPack CreatePack(PackManifest manifest)
    {
        return new AssetsPack(manifest);
    }
}