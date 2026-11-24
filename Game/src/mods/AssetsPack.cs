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
    /// <para>ReadAsset</para>
    /// <para>读取某个资源</para>
    /// </summary>
    /// <param name="virtualFileSystem"></param>
    /// <param name="paths"></param>
    /// <returns></returns>
    public byte[]? ReadAsset(VirtualFileSystem virtualFileSystem, params string[] paths)
    {
        return virtualFileSystem.ReadAllBytes(_packRootPath.Combine(paths));
    }

    protected override void StartLoadMessage(GameAppContext appContext, Action<PackMessage> onMessage)
    {
        onMessage.Invoke(new PackMessage("Load Assets Pack:{0}...", Manifest.Id));
    }
}