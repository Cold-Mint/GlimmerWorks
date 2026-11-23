using GlimmerWorks.engine.extensions;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods.PackLoader;

/// <summary>
/// <para>PackLoader</para>
/// <para>包加载器</para>
/// </summary>
/// <typeparam name="T"></typeparam>
public abstract class PackManager<T> where T : BasePack
{
    private readonly Dictionary<string, T> _packDictionary = new();
    private readonly Dictionary<string, VirtualPath> _idToVirtualPath = new();
    private bool _scaning;

    /// <summary>
    /// <para>GetPack</para>
    /// <para>获取某个包</para>
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    public T? GetPack(string id)
    {
        return _packDictionary.GetValueOrDefault(id);
    }

    /// <summary>
    /// <para>Scan</para>
    /// <para>扫描某个目录的包</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="virtualPath"></param>
    /// <param name="onMessage"></param>
    /// <returns>
    ///<para>The scanned quantity, if it is -1, indicates that the scanning process failed.</para>
    ///<para>扫描后的数量，如果为-1，那么表示扫描失败。</para>
    /// </returns>
    public async Task<int> ScanAndLoad(AppContext appContext, VirtualPath virtualPath, Action<PackMessage> onMessage)
    {
        if (_scaning)
        {
            return -1;
        }

        return await Task.Run(() =>
        {
            _scaning = true;
            _packDictionary.Clear();
            var fileEntryList = appContext.VirtualFileSystem.List(virtualPath);
            if (fileEntryList == null)
            {
                return 0;
            }

            List<T> packs = [];
            foreach (var fileEntry in fileEntryList)
            {
                if (!fileEntry.IsDirectory)
                {
                    continue;
                }

                var path = fileEntry.Path;
                if (path == null)
                {
                    continue;
                }

                var pack = LoadPackTemplate(appContext, path, onMessage);
                if (pack == null)
                {
                    continue;
                }

                packs.Add(pack);
            }

            foreach (var basePack in packs)
            {
                basePack.RegisterResourceType(appContext, basePack.ResourceTypeDictionary);
                basePack.Load(appContext, onMessage);
            }

            _scaning = false;
            return packs.Count;
        });
    }

    /// <summary>
    /// <para>LoadPackTemplate</para>
    /// <para>加载包内的模板</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="path"></param>
    /// <param name="onMessage"></param>
    /// <returns></returns>
    private T? LoadPackTemplate(AppContext appContext, VirtualPath path, Action<PackMessage> onMessage)
    {
        var manifestPath = path.Combine("manifest.toml");
        if (!appContext.VirtualFileSystem.FileExists(manifestPath))
        {
            return null;
        }

        var packManifest = appContext.VirtualFileSystem.ReadToml<PackManifest>(manifestPath);
        if (packManifest == null)
        {
            return null;
        }

        if (packManifest.MinGameCode > Constant.GameCode)
        {
            LogCat.Warn("The minimum game version is higher than the current game version.");
            return null;
        }

        if (_idToVirtualPath.TryGetValue(packManifest.Id, out var oldPath))
        {
            onMessage.Invoke(new PackMessage(
                appContext.Localizer[
                    "Due to the duplicate package ID, loading is prohibited. The solution is as follows:\n1. Modify the manifest.toml file located in the {0} directory and change the ID to {2} to achieve simultaneous loading.\n2. Remove the package located at {0} or remove the package located at {1}.",
                    path.Path, oldPath.Path, $"{packManifest.Id}_alt"])
            {
                Path = manifestPath,
                Level = LogLevel.Error
            });
            return null;
        }

        if (!packManifest.Id.IsSnakeCase())
        {
            onMessage.Invoke(new PackMessage(
                appContext.Localizer["{0} must use snake_case (expected: {1}, actual: {2}).", "id",
                    packManifest.Id.ToSnakeCase(), packManifest.Id].Value)
            {
                Path = manifestPath,
                Level = LogLevel.Error
            });
            return null;
        }

        if (!CheckManifest(appContext, manifestPath, onMessage, packManifest))
        {
            return null;
        }

        var pack = CreatePack(path, packManifest);
        _packDictionary[packManifest.Id] = pack;
        _idToVirtualPath[packManifest.Id] = path;
        pack.LoadAllTemplate(appContext, onMessage);
        return pack;
    }

    /// <summary>
    /// <para>CheckManifest</para>
    /// <para>检查清单文件是否合法</para>
    /// </summary>
    /// <param name="onMessage"></param>
    /// <param name="packManifest"></param>
    /// <param name="appContext"></param>
    /// <param name="manifestPath"></param>
    /// <returns></returns>
    protected virtual bool CheckManifest(AppContext appContext, VirtualPath manifestPath, Action<PackMessage> onMessage,
        PackManifest packManifest)
    {
        return true;
    }

    protected abstract T CreatePack(VirtualPath packRootPath, PackManifest manifest);
}