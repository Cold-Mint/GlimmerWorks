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
    private readonly Dictionary<(string PackId, Type Type, string ResourceId), Resource?> _resources = new();

    /// <summary>
    /// <para>packDependencies</para>
    /// <para>包依赖关系</para>
    /// </summary>
    private readonly Dictionary<string, HashSet<string>> _packDependencies = new();

    /// <summary>
    /// <para>SetDependencies</para>
    /// <para>设置包依赖</para>
    /// </summary>
    /// <param name="packId"></param>
    /// <param name="dependencies"></param>
    void SetDependencies(string packId, IEnumerable<string> dependencies)
    {
        if (string.IsNullOrWhiteSpace(packId))
        {
            return;
        }

        _packDependencies[packId] = [.. dependencies];
    }

    /// <summary>
    /// <para>Register</para>
    /// <para>注册资源</para>
    /// </summary>
    /// <param name="resource"></param>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    public bool Register<T>(T resource) where T : Resource
    {
        if (string.IsNullOrWhiteSpace(resource.PackId))
        {
            return false;
        }

        return !string.IsNullOrWhiteSpace(resource.ResourceId) &&
               _resources.TryAdd((resource.PackId, resource.GetType(), resource.ResourceId), resource);
    }

    /// <summary>
    /// <para>Register</para>
    /// <para>注册资源</para>
    /// </summary>
    /// <param name="resource"></param>
    /// <returns></returns>
    public bool Register(Resource resource)
    {
        if (string.IsNullOrWhiteSpace(resource.PackId))
        {
            return false;
        }

        return !string.IsNullOrWhiteSpace(resource.ResourceId) &&
               _resources.TryAdd((resource.PackId, resource.GetType(), resource.ResourceId), resource);
    }


    /// <summary>
    /// <para></para>
    /// <para>Get</para>
    /// <para>获取某个资源</para>
    /// </summary>
    /// <param name="resourceRef"></param>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    public T? Get<T>(ResourceRef resourceRef) where T : Resource
    {
        if (resourceRef.RequesterPackId != resourceRef.TargetPackId &&
            _packDependencies.TryGetValue(resourceRef.RequesterPackId, out var dependencies))
        {
            //If it is a cross-package access, then check the dependencies.
            //如果是跨包访问，那么检查依赖。
            if (!dependencies.Contains(resourceRef.ResourceId))
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