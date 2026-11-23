using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Game.Mods;

public class AssetsPack(VirtualPath packRootPath, PackManifest manifest)
    : BasePack<GameAppContext>(packRootPath, manifest)
{
    protected override bool ShouldScanFiles()
    {
        return false;
    }

    protected override void StartLoadMessage(GameAppContext appContext, Action<PackMessage> onMessage)
    {
        onMessage.Invoke(new PackMessage("Load Assets Pack:{0}...", Manifest.Id));
    }
}