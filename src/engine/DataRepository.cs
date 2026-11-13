using System.Reflection;
using GlimmerWorks.engine;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Data;

namespace GlimmerWorks.Engine;

/// <summary>
/// <para>DataRepository</para>
/// <para>数据仓库</para>
/// </summary>
public class DataRepository
{
    /// <summary>
    /// <para>Resource Dictionary</para>
    /// <para>资源字典</para>
    /// </summary>
    private readonly Dictionary<(string PackId, Type Type, string ResourceId), Resources?> _resources = new();

    private readonly Dictionary<Type, List<Resources>> _typeToResources = new();

    private readonly Dictionary<string, Type> _resourceTypeDictionary = new();

    /// <summary>
    /// <para>packDependencies</para>
    /// <para>包依赖关系</para>
    /// </summary>
    private readonly Dictionary<string, HashSet<string>> _packDependencies = new();

    public DataRepository()
    {
        var resourceType = typeof(Resources);
        foreach (var type in resourceType.Assembly.GetTypes())
        {
            if (type.IsAbstract || !resourceType.IsAssignableFrom(type))
            {
                continue;
            }

            var attr = type.GetCustomAttribute<DataPackResource>();
            if (attr is null)
            {
                continue;
            }

            if (!attr.AutoDeserialization)
            {
                continue;
            }

            _resourceTypeDictionary.Add(attr.ResourceType, type);
        }
    }

    /// <summary>
    /// <para>SetDependencies</para>
    /// <para>设置包依赖</para>
    /// </summary>
    /// <param name="targetPackId"></param>
    /// <param name="dependencies"></param>
    void SetDependencies(string targetPackId, IEnumerable<string> dependencies)
    {
        if (string.IsNullOrWhiteSpace(targetPackId))
        {
            return;
        }

        _packDependencies[targetPackId] = [.. dependencies];
    }

    private void AddToTypeIndex(Resources resource)
    {
        var t = resource.GetType();
        if (!_typeToResources.TryGetValue(t, out var list))
        {
            list = new List<Resources>();
            _typeToResources[t] = list;
        }

        list.Add(resource);
    }

    /// <summary>
    /// <para>GetResourceList</para>
    /// <para>获取资源列表</para>
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    public IEnumerable<T> GetResourceList<T>(Func<T, bool>? filter = null) where T : Resources
    {
        if (!_typeToResources.TryGetValue(typeof(T), out var list))
        {
            yield break;
        }

        foreach (var r in list)
        {
            if (r is T t && (filter is null || filter(t)))
            {
                yield return t;
            }
        }
    }

    /// <summary>
    /// <para>Get a resource by filter</para>
    /// <para>按照过滤器获取一个资源</para>
    /// </summary>
    /// <param name="filter"></param>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    public T? FirstOrDefaultResources<T>(Func<T, bool>? filter = null) where T : Resources
    {
        return GetResourceList(filter).FirstOrDefault();
    }

    /// <summary>
    /// <para>Register</para>
    /// <para>注册资源</para>
    /// </summary>
    /// <param name="resource"></param>
    /// <returns></returns>
    public bool Register(Resources resource)
    {
        if (string.IsNullOrWhiteSpace(resource.PackId) || string.IsNullOrWhiteSpace(resource.ResourceId))
        {
            return false;
        }

        if (_resources.TryAdd((resource.PackId, resource.GetType(), resource.ResourceId), resource))
        {
            AddToTypeIndex(resource);
            return true;
        }

        return false;
    }


    /// <summary>
    /// <para></para>
    /// <para>Get</para>
    /// <para>获取某个资源</para>
    /// </summary>
    /// <param name="resourceRef"></param>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    public T? Get<T>(ResourceRef resourceRef) where T : Resources
    {

        if (!_resourceTypeDictionary.TryGetValue(resourceRef.ResourceType, out var actualType))
        {
            return null;
        }
        
        if (resourceRef.RequesterPackId != resourceRef.TargetPackId &&
            _packDependencies.TryGetValue(resourceRef.RequesterPackId, out var dependencies))
        {
            //If it is a cross-package access, then check the dependencies.
            //如果是跨包访问，那么检查依赖。
            if (!dependencies.Contains(resourceRef.TargetPackId))
            {
                //Prohibit cross-package access.
                //禁止跨包访问。
                return null;
            }
        }

        if (resourceRef.IsValid && _resources.TryGetValue((resourceRef.TargetPackId, typeof(T), resourceRef.ResourceId),
                out var resource))
        {
            return resource as T;
        }

        return null;
    }
}