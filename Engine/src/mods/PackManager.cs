using GlimmerWorks.Engine.Extensions;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Engine.Mods;

/// <summary>
/// <para>PackManager</para>
/// <para>包管理器</para>
/// </summary>
/// <typeparam name="TPack"></typeparam>
/// <typeparam name="TAppContext"></typeparam>
public abstract class PackManager<TAppContext, TPack> where TAppContext : AppContext where TPack : BasePack<TAppContext>
{
    private readonly Dictionary<string, TPack> _packDictionary = new();
    private readonly Dictionary<string, VirtualPath> _idToVirtualPath = new();
    private readonly List<TPack> _packs = [];
    private bool _scaning;

    protected IEnumerable<TPack> GetPacks(
        Func<TPack, bool>? filter = null,
        IComparer<TPack>? comparer = null)
    {
        IEnumerable<TPack> query = _packs;
        if (filter is not null)
        {
            query = query.Where(filter);
        }

        if (comparer is not null)
        {
            query = query.OrderBy(p => p, comparer);
        }

        return query;
    }

    /// <summary>
    /// <para>The number of loaded packs.</para>
    /// <para>已加载的包数量。</para>
    /// </summary>
    public int PackCount => _packs.Count;

    /// <summary>
    /// <para>GetPack</para>
    /// <para>获取某个包</para>
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    public TPack? GetPack(string id)
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
    public async Task<int> ScanAndLoad(TAppContext appContext, VirtualPath virtualPath, Action<PackMessage> onMessage)
    {
        if (_scaning)
        {
            return -1;
        }

        return await Task.Run(() =>
        {
            _scaning = true;
            _packs.Clear();
            _idToVirtualPath.Clear();
            _packDictionary.Clear();
            var fileEntryList = appContext.VirtualFileSystem.List(virtualPath);
            if (fileEntryList == null)
            {
                return 0;
            }

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

                var packManifest = ReadAndCheckManifest(appContext, path, onMessage);
                if (packManifest == null)
                {
                    continue;
                }

                var pack = CreatePack(path, packManifest);
                pack.LoadAllTemplate(appContext, onMessage);
                _packDictionary[packManifest.Id] = pack;
                _idToVirtualPath[packManifest.Id] = path;
                _packs.Add(pack);
            }

            foreach (var basePack in _packs)
            {
                basePack.RegisterResourceType(appContext, basePack.ResourceTypeDictionary);
                basePack.Load(appContext, onMessage);
            }

            //Apply patch resources.
            //应用补丁资源。
            appContext.DataRepository.ApplyPatch();
            _scaning = false;
            return _packs.Count;
        });
    }

    /// <summary>
    /// <para>ExtraChecklist</para>
    /// <para>额外检查清单文件</para>
    /// </summary>
    /// <param name="packManifest"></param>
    /// <param name="manifestPath"></param>
    /// <param name="onMessage"></param>
    /// <returns></returns>
    protected virtual bool ExtraCheckManifest(PackManifest packManifest, VirtualPath manifestPath,
        Action<PackMessage> onMessage)
    {
        return true;
    }


    /// <summary>
    /// <para>ReadAndCheckManifest</para>
    /// <para>读取并检查清单文件</para>
    /// </summary>
    /// <param name="onMessage"></param>
    /// <param name="appContext"></param>
    /// <param name="path"></param>
    /// <returns></returns>
    private PackManifest? ReadAndCheckManifest(TAppContext appContext, VirtualPath path,
        Action<PackMessage> onMessage)
    {
        var manifestPath = path.Combine("manifest.toml");
        var packManifest = appContext.VirtualFileSystem.ReadToml<PackManifest>(manifestPath);
        if (packManifest == null)
        {
            onMessage.Invoke(new PackMessage(
                "The manifest.toml file cannot be read.")
            {
                Path = manifestPath,
                Level = LogLevels.Warn
            });
            return null;
        }

        if (packManifest.MinGameCode > EngineConstant.GameCode)
        {
            onMessage.Invoke(new PackMessage(
                "The minimum game version is higher than the current game version.")
            {
                Path = manifestPath,
                Level = LogLevels.Warn
            });
            return null;
        }

        if (_idToVirtualPath.TryGetValue(packManifest.Id, out var oldPath))
        {
            onMessage.Invoke(new PackMessage(
                "Due to the duplicate package ID, loading is prohibited. The solution is as follows:\n1. Modify the manifest.toml file located in the {0} directory and change the ID to {2} to achieve simultaneous loading.\n2. Remove the package located at {0} or remove the package located at {1}.",
                path.Path, oldPath.Path, $"{packManifest.Id}_alt")
            {
                Path = manifestPath,
                Level = LogLevels.Error
            });
            return null;
        }

        if (!packManifest.Id.IsSnakeCase())
        {
            onMessage.Invoke(new PackMessage(
                "{0} must use snake_case (expected: {1}, actual: {2}).", "id",
                packManifest.Id.ToSnakeCase(), packManifest.Id)
            {
                Path = manifestPath,
                Level = LogLevels.Error
            });
        }

        return ExtraCheckManifest(packManifest, manifestPath, onMessage) ? packManifest : null;
    }

    protected abstract TPack CreatePack(VirtualPath packRootPath, PackManifest manifest);
}