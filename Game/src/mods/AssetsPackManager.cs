using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.PackLoader;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Game.Mods;

/// <summary>
/// <para>AssetsPackLoader</para>
/// <para>资产包加载器</para>
/// </summary>
public class AssetsPackManager : PackManager<GameAppContext, AssetsPack>
{
    protected override bool ExtraCheckManifest(PackManifest packManifest, VirtualPath manifestPath,
        Action<PackMessage> onMessage)
    {
        if (packManifest.ResPack)
        {
            return true;
        }

        onMessage.Invoke(new PackMessage(
            "Please add \"res_pack = true\" in the file. To solve this error.")
        {
            Path = manifestPath,
            Level = LogLevel.Error
        });
        return false;
    }

    protected override AssetsPack CreatePack(VirtualPath packRootPath, PackManifest manifest)
    {
        return new AssetsPack(packRootPath, manifest);
    }
}