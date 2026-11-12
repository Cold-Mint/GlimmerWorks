using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods.PackLoader;

/// <summary>
/// <para>AssetsPackLoader</para>
/// <para>资产包加载器</para>
/// </summary>
public class AssetsPackLoader : PackLoader<AssetsPack>
{
    protected override AssetsPack CreatePack(VirtualPath packRootPath, PackManifest manifest)
    {
        return new AssetsPack(packRootPath, manifest);
    }
}