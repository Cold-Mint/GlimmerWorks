using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.PackLoader;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Game.Mods;

/// <summary>
/// <para>DataPackLoader</para>
/// <para>数据包加载器</para>
/// </summary>
public class DataPackManager : PackManager<GameAppContext, LocalDataPack>
{
    protected override bool ExtraCheckManifest(PackManifest packManifest, VirtualPath manifestPath,
        Action<PackMessage> onMessage)
    {
        if (packManifest.ResPack)
        {
            onMessage.Invoke(new PackMessage(
                "Please set \"res_pack = false\" in the file. To solve this error.")
            {
                Path = manifestPath,
                Level = LogLevels.Error
            });
            return false;
        }

        return true;
    }

    protected override LocalDataPack CreatePack(VirtualPath packRootPath, PackManifest manifest)
    {
        return new LocalDataPack(packRootPath, manifest);
    }
}