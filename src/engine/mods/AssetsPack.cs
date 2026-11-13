using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods;

public class AssetsPack(VirtualPath packRootPath, PackManifest manifest) : BasePack(packRootPath, manifest)
{
    protected override bool ShouldScanFiles()
    {
        return false;
    }

    protected override string StartLoadMessage()
    {
        return "Load Assets Pack:{0}...";
    }
}