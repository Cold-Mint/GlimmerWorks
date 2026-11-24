using Blake3;
using GlimmerWorks.Engine.Extensions;
using GlimmerWorks.Engine.Vfs;
using MemoryPack;

namespace GlimmerWorks.Engine.Cache;

public class CacheManager
{
    private readonly Dictionary<string, string> _pathToId = new();
    

    /// <summary>
    /// <para>GetCacheEntry</para>
    /// <para>获取缓存实体</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="cacheType"></param>
    /// <param name="path"></param>
    /// <returns></returns>
    private CacheEntry? GetCacheEntry(AppContext appContext, string cacheType,
        VirtualPath path)
    {
        if (!_pathToId.TryGetValue(path.Path, out var uuid))
        {
            return null;
        }

        var bytes = appContext.VirtualFileSystem.ReadAllBytes(GetCachePath(appContext, cacheType, uuid));
        if (bytes == null)
        {
            return null;
        }

        return MemoryPackSerializer.Deserialize<CacheEntry>(bytes);
    }

    public bool CacheAvailable(AppContext appContext, string cacheType, VirtualPath path)
    {
        var cacheEntry = GetCacheEntry(appContext, cacheType, path);
        if (cacheEntry == null)
        {
            return false;
        }

        var virtualFileSystem = appContext.VirtualFileSystem;
        var fileEntry = virtualFileSystem.GetFileEntry(path);
        if (fileEntry == null || fileEntry.IsDirectory || !virtualFileSystem.FileExists(path))
        {
            return false;
        }

        // Fast path: timestamp and size are unchanged, so the content is treated as unchanged.
        // 快速路径：修改时间和大小都没变，认为内容未变化。
        if (cacheEntry.Size is not null &&
            cacheEntry.Size == fileEntry.Size &&
            cacheEntry.LastWriteTimeUtc == fileEntry.LastWriteTimeUtc)
        {
            return true;
        }

        // Slow path: metadata changed, compare the content hash to avoid invalidating on touch/copy.
        // 慢速路径：元数据变了，再比对内容哈希，避免仅仅 touch 或复制文件就失效。
        var bytes = virtualFileSystem.ReadAllBytes(path);
        if (bytes == null)
        {
            return false;
        }

        var hash = Hasher.Hash(bytes);
        return hash.AsSpan().SequenceEqual(cacheEntry.Blake3);
    }

    /// <summary>
    /// <para>MakeCache</para>
    /// <para>为某个文件建立缓存</para>
    /// </summary>
    /// <param name="appContext"></param>
    /// <param name="cacheType"></param>
    /// <param name="path"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    public bool MakeCache(AppContext appContext, string cacheType,
        VirtualPath path, byte[]? data = null)
    {
        if (_pathToId.ContainsKey(path.Path))
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
            Data = data
        };
        _pathToId.Add(path.Path, uuid);
        appContext.VirtualFileSystem.WriteAllBytes(GetCachePath(appContext, cacheType, uuid),
            MemoryPackSerializer.Serialize(cacheEntry));
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
    private VirtualPath GetCachePath(AppContext appContext, string cacheType, string cacheKey)
    {
        var typedCacheKey = $"{cacheKey}.cache";
        return new VirtualPath([
            .. appContext.EngineConfig.CachePath.SplitPath(),
            cacheType, .. typedCacheKey.SplitPath()
        ]);
    }
}