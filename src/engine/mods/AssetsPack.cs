using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods;

public class AssetsPack(VirtualPath packRootPath, PackManifest manifest) : BasePack(packRootPath, manifest)
{
    protected override bool ShouldScanFiles()
    {
        return false;
    }


    protected override void StartLoadMessage(AppContext appContext, Action<PackMessage> onMessage)
    {
        base.StartLoadMessage(appContext, onMessage);
        onMessage.Invoke(new PackMessage(appContext.Localizer["Load Assets Pack:{0}...", Manifest.Id].Value));
    }
}