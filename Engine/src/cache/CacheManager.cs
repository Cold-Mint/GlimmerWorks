using Blake3;
using GlimmerWorks.Engine.Extensions;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Engine.Cache;

public class CacheManager
{
    private CacheManifest _manifest = new();

    /// <summary>
    /// <para>Read the list file stored in the cache</para>
    /// <para>读取缓存的清单文件</para>
    /// </summary>
    /// <param name="appContext"></param>
    public bool ReadCacheManifest(AppContext appContext)
    {
        var virtualPath = GetCacheManifestPath(appContext.EngineConfig.CachePath);
        var fileEntry = appContext.VirtualFileSystem.GetFileEntry(virtualPath);
        if (fileEntry == null || fileEntry.IsDirectory || !appContext.VirtualFileSystem.FileExists(virtualPath))
        {
            return false;
        }

        var cacheManifest = appContext.VirtualFileSystem.ReadMemoryPack<CacheManifest>(virtualPath);
        if (cacheManifest == null || cacheManifest.GameCode != EngineConstant.GameCode)
        {
            return false;
        }

        _manifest = cacheManifest;
        return true;
    }

    /// <summary>
    /// <para>WriteCacheManifest</para>
    /// <para>写入缓存清单文件</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <returns></returns>
    public bool WriteCacheManifest(AppContext appContext)
    {
        var virtualPath = GetCacheManifestPath(appContext.EngineConfig.CachePath);
        return appContext.VirtualFileSystem.WriteMemoryPack(virtualPath, _manifest);
    }

    /// <summary>
    /// <para>GetCacheManifestPath</para>
    /// <para>获取缓存的清单路径</para>
    /// </summary>
    /// <param name="cachePath"></param>
    /// <returns></returns>
    private static VirtualPath GetCacheManifestPath(string cachePath)
    {
        return new VirtualPath([.. cachePath.SplitPath(), "manifest.bin"]);
    }

    /// <summary>
    /// <para>GetCacheEntry</para>
    /// <para>获取缓存实体</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="cacheType"></param>
    /// <param name="path"></param>
    /// <returns></returns>
    private CacheEntry? ReadCacheEntry(AppContext appContext, string cacheType,
        VirtualPath path)
    {
        if (!_manifest.PathToId.TryGetValue(path.Path, out var uuid))
        {
            return null;
        }

        return appContext.VirtualFileSystem.ReadMemoryPack<CacheEntry>(GetCachePath(appContext, cacheType, uuid));
    }

    /// <summary>
    /// <para>GetCacheIfAvailable</para>
    /// <para>获取缓存</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="cacheType"></param>
    /// <param name="path"></param>
    /// <returns>
    ///<para>If the cache is unavailable, return null.</para>
    ///<para>如果缓存不可用返回null。</para>
    /// </returns>
    public CacheEntry? GetCacheIfAvailable(AppContext appContext, string cacheType, VirtualPath path)
    {
        var cacheEntry = ReadCacheEntry(appContext, cacheType, path);
        if (cacheEntry == null)
        {
            return null;
        }

        var virtualFileSystem = appContext.VirtualFileSystem;
        var fileEntry = virtualFileSystem.GetFileEntry(path);
        if (fileEntry == null || fileEntry.IsDirectory || !virtualFileSystem.FileExists(path))
        {
            return null;
        }

        // Fast path: timestamp and size are unchanged, so the content is treated as unchanged.
        // 快速路径：修改时间和大小都没变，认为内容未变化。
        if (cacheEntry.Size is not null &&
            cacheEntry.Size == fileEntry.Size &&
            cacheEntry.LastWriteTimeUtc == fileEntry.LastWriteTimeUtc)
        {
            return cacheEntry;
        }

        // Slow path: metadata changed, compare the content hash to avoid invalidating on touch/copy.
        // 慢速路径：元数据变了，再比对内容哈希，避免仅仅 touch 或复制文件就失效。
        var bytes = virtualFileSystem.ReadAllBytes(path);
        if (bytes == null)
        {
            return null;
        }

        var hash = Hasher.Hash(bytes);
        if (hash.AsSpan().SequenceEqual(cacheEntry.Blake3))
        {
            return cacheEntry;
        }

        return null;
    }

    /// <summary>
    /// <para>MakeCache</para>
    /// <para>为某个文件建立缓存</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="cacheType"></param>
    /// <param name="path"></param>
    /// <param name="data"></param>
    /// <param name="assetsPackId"></param>
    /// <returns></returns>
    public bool MakeCache(AppContext appContext, string cacheType,
        VirtualPath path, byte[] data, string assetsPackId)
    {
        if (_manifest.PathToId.ContainsKey(path.Path))
        {
            return false;
        }

        var fileEntry = appContext.VirtualFileSystem.GetFileEntry(path);
        if (fileEntry == null || fileEntry.IsDirectory || !appContext.VirtualFileSystem.FileExists(path))
        {
            return false;
        }

        var bytes = appContext.VirtualFileSystem.ReadAllBytes(path);
        if (bytes == null)
        {
            return false;
        }

        var hash = Hasher.Hash(bytes);
        var guid = Guid.NewGuid();
        var uuid = guid.ToString("N");
        var cacheEntry = new CacheEntry
        {
            LastWriteTimeUtc = fileEntry.LastWriteTimeUtc,
            Size = fileEntry.Size,
            Blake3 = [.. hash.AsSpan()],
            AssetsPackId = assetsPackId,
            Data = data
        };
        _manifest.PathToId.Add(path.Path, uuid);
        appContext.VirtualFileSystem.WriteMemoryPack(GetCachePath(appContext, cacheType, uuid), cacheEntry);
        return true;
    }

    /// <summary>
    /// <para>GetCachePath</para>
    /// <para>获取缓存路径</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="cacheType"></param>
    /// <param name="cacheKey"></param>
    /// <returns></returns>
    private static VirtualPath GetCachePath(AppContext appContext, string cacheType, string cacheKey)
    {
        var typedCacheKey = $"{cacheKey}.cache";
        return new VirtualPath([
            .. appContext.EngineConfig.CachePath.SplitPath(),
            cacheType, .. typedCacheKey.SplitPath()
        ]);
    }
}