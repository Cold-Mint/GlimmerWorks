using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Game.Mods;

public class AssetsPack(VirtualPath packRootPath, PackManifest manifest)
    : BasePack<GameAppContext>(packRootPath, manifest)
{
    private readonly VirtualPath _packRootPath = packRootPath;

    protected override bool ShouldScanFiles()
    {
        return false;
    }

    /// <summary>
    /// <para>GetAssetsPath</para>
    /// <para>获取资产的路径</para>
    /// </summary>
    /// <param name="paths"></param>
    /// <returns></returns>
    public VirtualPath GetAssetsPath(params string[] paths)
    {
        return _packRootPath.Combine(paths);
    }

    protected override void StartLoadMessage(GameAppContext appContext, Action<PackMessage> onMessage)
    {
        onMessage.Invoke(new PackMessage("Load Assets Pack:{0}...", Manifest.Id));
    }
}