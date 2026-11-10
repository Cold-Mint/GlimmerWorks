using GlimmerWorks.Engine.Mods.Data;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods;

public class DataPack(PackManifest manifest) : BasePack(manifest)
{
    protected override bool ShouldScanFiles()
    {
        return true;
    }

    protected override void RegisterResourceType(Dictionary<string, Type> resourceTypeDictionary)
    {
        base.RegisterResourceType(resourceTypeDictionary);
        resourceTypeDictionary.Add("dimension", typeof(DimensionResource));
    }

    protected override string? StartLoadMessage()
    {
        return "ui.load_data_pack";
    }
}