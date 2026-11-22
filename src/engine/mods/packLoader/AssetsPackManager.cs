using GlimmerWorks.Engine.Log;
using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods.PackLoader;

/// <summary>
/// <para>AssetsPackLoader</para>
/// <para>资产包加载器</para>
/// </summary>
public class AssetsPackManager : PackManager<AssetsPack>
{
    protected override bool CheckManifest(AppContext appContext, VirtualPath manifestPath,
        Action<PackMessage> onMessage, PackManifest packManifest)
    {
        if (packManifest.ResPack)
        {
            return base.CheckManifest(appContext, manifestPath, onMessage, packManifest);
        }

        onMessage.Invoke(new PackMessage(
            appContext.Localizer["Please add \"res_pack = true\" in the file. To solve this error."].Value)
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