using GlimmerWorks.Engine.Log;
using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods.PackLoader;

/// <summary>
/// <para>DataPackLoader</para>
/// <para>数据包加载器</para>
/// </summary>
public class DataPackManager : PackManager<DataPack>
{
    protected override bool CheckManifest(AppContext appContext, VirtualPath manifestPath, Action<PackMessage> onMessage, PackManifest packManifest)
    {
        if (packManifest.ResPack)
        {
            onMessage.Invoke(new PackMessage(
                appContext.Localizer["Please set \"res_pack = false\" in the file. To solve this error."].Value)
            {
                Path = manifestPath,
                Level = LogLevel.Error
            });
            return false;
        }
        return base.CheckManifest(appContext, manifestPath, onMessage, packManifest);
    }

    protected override DataPack CreatePack(VirtualPath packRootPath, PackManifest manifest)
    {
        return new DataPack(packRootPath, manifest);
    }
}