using System.Reflection;
using GlimmerWorks.Engine.Extensions;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.PackLoader;
using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Attributes;
using MemoryPack;

namespace GlimmerWorks.Game.Mods;

/// <summary>
/// <para>AssetsPackManager</para>
/// <para>资源包管理器</para>
/// </summary>
public class AssetsPackManager : PackManager<GameAppContext, AssetsPack>, IComparer<AssetsPack>
{
    /// <summary>
    /// <para>The list of reading orders for the resource packages (with "string" being AssetsPack.Manifest.id)</para>
    /// <para>资源包的读取顺序（string为AssetsPack.Manifest.id）的列表</para>
    /// </summary>
    /// <remarks>
    ///<para>For example: When searching for a certain resource, the search will start from the top of the list and proceed downwards. If the resource package at the top returns the corresponding resource, then the bottom part will not be read. If no resource is found at either the top or the bottom, then it will return that the resource is not found.</para>
    ///<para>例如：当查找某个资源时，会从列表的顶部向底部查找，如果上面的资源包返回了对应的资源，那么就不在读取底部的，如果都没读到那么返回找不到资源</para>
    /// </remarks>
    public List<string> Order { get; set; } = new();

    /// <summary>
    /// <para>_assetsParser</para>
    /// <para>资产解析器</para>
    /// </summary>
    private readonly Dictionary<string, object> _assetsParserDictionary = new();

    public AssetsPackManager()
    {
        var assembly = Assembly.GetExecutingAssembly();
        foreach (var type in assembly.GetTypes())
        {
            if (type.IsAbstract || type.IsInterface)
            {
                continue;
            }
            var attr = type.GetCustomAttribute<AssetsPasterAttribute>();
            if (attr is null)
            {
                continue;
            }
            
            var assetsParser = Activator.CreateInstance(type);
            if (assetsParser == null)
            {
                continue;
            }

            if (!_assetsParserDictionary.TryAdd(attr.ResourceType, assetsParser))
            {
                LogCat.Error(
                    $"AssetsPackManager has the same resourceType: {attr.ResourceType}, type: {type.FullName ?? type.Name}");
                continue;
            }

            LogCat.Info($"Register assetsParser: {attr.ResourceType}");
        }
    }

    protected override bool ExtraCheckManifest(PackManifest packManifest, VirtualPath manifestPath,
        Action<PackMessage> onMessage)
    {
        if (packManifest.ResPack)
        {
            return true;
        }

        onMessage.Invoke(new PackMessage(
            "Please add \"res_pack = true\" in the file. To solve this error.")
        {
            Path = manifestPath,
            Level = LogLevels.Error
        });
        return false;
    }

    private static T? TryLoadAssets<T>(GameAppContext appContext, string resourceType,
        VirtualPath assetsPath, string assetsPackId,
        BaseAssetsParser<T> parser)
    {
        if (!appContext.VirtualFileSystem.FileExists(assetsPath))
        {
            return default;
        }

        if (parser.EnableFileCache())
        {
            var cacheEntry =
                appContext.CacheManager.GetCacheIfAvailable(appContext, resourceType,
                    assetsPath);
            var cacheData = cacheEntry?.Data;
            if (cacheData != null)
            {
                var assetObj = MemoryPackSerializer.Deserialize<T>(cacheData);
                if (!Equals(assetObj, default(T)))
                {
                    //Success: Data has been recovered from the cache.
                    //成功从缓存中恢复。
                    return assetObj;
                }
            }
        }

        var fileEntry = appContext.VirtualFileSystem.GetFileEntry(assetsPath);
        if (fileEntry == null)
        {
            return default;
        }

        var asset = parser.Parser(appContext, fileEntry, assetsPackId);
        if (Equals(asset, default(T)))
        {
            return default;
        }

        if (parser.EnableFileCache())
        {
            appContext.CacheManager.MakeCache(appContext, resourceType, assetsPath,
                MemoryPackSerializer.Serialize(asset), assetsPackId);
        }

        return asset;
    }

    public AssetResult<T> FindAsset<T>(GameAppContext appContext, ResourceRef resourceRef)
    {
        var result = new AssetResult<T>();
        if (!resourceRef.IsValid)
        {
            return result;
        }

        var filePath = resourceRef.ResourceId.SplitPath();
        if (_assetsParserDictionary.TryGetValue(resourceRef.ResourceType, out var parser))
        {
            var typedAssetsParser = parser as BaseAssetsParser<T>;
            if (typedAssetsParser == null)
            {
                return result;
            }

            filePath[^1] += "." + typedAssetsParser.GetFileTypeName();
            foreach (var assetsPack in GetPacks(comparer: this))
            {
                var assetsPath =
                    assetsPack.GetAssetsPath([
                        typedAssetsParser.GetFolderName(), resourceRef.TargetPackId, .. filePath
                    ]);
                var assets = TryLoadAssets(appContext, resourceRef.ResourceType, assetsPath, assetsPack.Manifest.Id,
                    typedAssetsParser);
                if (Equals(assets, default(T)))
                {
                    continue;
                }
                result.Result = assets;
                return result;
            }
        }

        return result;
    }

    protected override AssetsPack CreatePack(VirtualPath packRootPath, PackManifest manifest)
    {
        return new AssetsPack(packRootPath, manifest);
    }

    public int Compare(AssetsPack? x, AssetsPack? y)
    {
        if (ReferenceEquals(x, y))
        {
            return 0;
        }

        if (x is null)
        {
            return 1;
        }

        if (y is null)
        {
            return 1;
        }

        var ix = Order.IndexOf(x.Manifest.Id);
        var iy = Order.IndexOf(y.Manifest.Id);
        if (ix < 0)
        {
            ix = int.MaxValue;
        }

        if (iy < 0)
        {
            iy = int.MaxValue;
        }

        return ix.CompareTo(iy);
    }
}