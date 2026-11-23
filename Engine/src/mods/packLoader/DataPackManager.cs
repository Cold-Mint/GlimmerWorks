using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Engine.Mods.PackLoader;

/// <summary>
/// <para>DataPackLoader</para>
/// <para>数据包加载器</para>
/// </summary>
public class DataPackManager<TAppContext> : PackManager<TAppContext, DataPack<TAppContext>>
    where TAppContext : AppContext
{
    protected override bool CheckManifest(TAppContext appContext, VirtualPath manifestPath,
        Action<PackMessage> onMessage, PackManifest packManifest)
    {
        if (packManifest.ResPack)
        {
            onMessage.Invoke(new PackMessage(
                "Please set \"res_pack = false\" in the file. To solve this error.")
            {
                Path = manifestPath,
                Level = LogLevel.Error
            });
            return false;
        }

        return base.CheckManifest(appContext, manifestPath, onMessage, packManifest);
    }

    protected override DataPack<TAppContext> CreatePack(VirtualPath packRootPath, PackManifest manifest)
    {
        return new DataPack<TAppContext>(packRootPath, manifest);
    }
}