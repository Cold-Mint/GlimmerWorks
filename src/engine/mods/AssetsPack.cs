using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods;

public class AssetsPack(PackManifest manifest) : BasePack(manifest)
{
    protected override bool ShouldScanFiles()
    {
        return false;
    }

    protected override string? StartLoadMessage()
    {
        return "ui.load_assets_pack";
    }
}