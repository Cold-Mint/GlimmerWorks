using System.Reflection;
using GlimmerWorks.Engine.Log;

namespace GlimmerWorks.Engine.World.Ecs;

/// <summary>
/// <para>SystemScheduler</para>
/// <para>系统调度器。负责发现、创建并调度所有 <see cref="ISystem"/> 实现。</para>
/// </summary>
public class SystemScheduler
{
    /// <summary>
    /// <para>System List</para>
    /// <para>系统列表</para>
    /// </summary>
    private readonly List<ISystem> _disabledSystemList = new();

    /// <summary>
    /// <para>Activated System List</para>
    /// <para>激活的系统列表</para>
    /// </summary>
    private readonly List<ISystem> _activatedSystemList = new();

    public IReadOnlyList<ISystem> GetActivatedSystemList()
    {
        return _activatedSystemList;
    }

    /// <summary>
    /// <para>Scan the given assemblies for systems.</para>
    /// <para>扫描指定程序集，创建其中所有系统。</para>
    /// </summary>
    public SystemScheduler()
    {
        var assemblies = GetDefaultAssemblies();
        var systemInterface = typeof(ISystem);

        foreach (var assembly in assemblies.Distinct())
        {
            var systemTypes = GetLoadableTypes(assembly)
                .Where(static type => type is
                {
                    IsInterface: false,
                    IsAbstract: false,
                    IsGenericTypeDefinition: false
                })
                .Where(systemInterface.IsAssignableFrom)
                // Reflection does not guarantee declaration order; sort for a stable registration order.
                // 反射不保证声明顺序，排序以保证每次注册顺序一致。
                .OrderBy(static type => type.FullName, StringComparer.Ordinal);

            foreach (var type in systemTypes)
            {
                var system = CreateSystem(type);
                if (system is null)
                {
                    continue;
                }

                _disabledSystemList.Add(system);
                LogCat.Info($"Register system: {type.FullName ?? type.Name}");
            }
        }

        LogCat.Info($"System registration completed, total: {_disabledSystemList.Count}");
    }

    /// <summary>
    /// <para>Create one system instance, logging and skipping types that cannot be instantiated.</para>
    /// <para>创建一个系统实例；无法实例化的类型会记录日志并跳过。</para>
    /// </summary>
    private static ISystem? CreateSystem(Type type)
    {
        var typeName = type.FullName ?? type.Name;

        if (type.IsClass && type.GetConstructor(Type.EmptyTypes) is null)
        {
            LogCat.Error($"System has no public parameterless constructor: {typeName}");
            return null;
        }

        try
        {
            return Activator.CreateInstance(type) as ISystem;
        }
        catch (Exception exception) when (exception is TargetInvocationException or MemberAccessException)
        {
            LogCat.Error($"Failed to create system {typeName}: {exception.Message}");
            return null;
        }
    }

    /// <summary>
    /// <para>Get types from an assembly, keeping the loadable ones when some types fail to load.</para>
    /// <para>读取程序集中的类型；部分类型加载失败时保留可加载的类型。</para>
    /// </summary>
    private static IEnumerable<Type> GetLoadableTypes(Assembly assembly)
    {
        try
        {
            return assembly.GetTypes();
        }
        catch (ReflectionTypeLoadException exception)
        {
            LogCat.Error($"Failed to load some types from {assembly.FullName}: {exception.Message}");
            return exception.Types.OfType<Type>();
        }
    }

    /// <summary>
    /// <para>Default assemblies: the engine assembly plus the application entry assembly.</para>
    /// <para>默认扫描范围：引擎程序集以及应用程序入口程序集。</para>
    /// </summary>
    private static IEnumerable<Assembly> GetDefaultAssemblies()
    {
        var engineAssembly = typeof(ISystem).Assembly;
        yield return engineAssembly;

        var entryAssembly = Assembly.GetEntryAssembly();
        if (entryAssembly is not null && entryAssembly != engineAssembly)
        {
            yield return entryAssembly;
        }
    }


    /// <summary>
    /// <para>
    /// Move systems between the disabled and activated lists according to their dependent components.
    /// A system is activated only when every dependent component type currently has at least one instance.
    /// </para>
    /// <para>
    /// 根据系统依赖的组件，在禁用列表和激活列表之间移动系统。
    /// 只有所有依赖组件类型当前都至少存在一个实例时，系统才会被激活。
    /// </para>
    /// </summary>
    public void NeedActivate(WorldContext worldContext)
    {
        var entityManager = worldContext.EntityManager;
        // Use an index loop instead of foreach because items are removed while iterating.
        // 遍历过程中会移除元素，所以使用索引循环而不是 foreach。
        for (var i = 0; i < _disabledSystemList.Count;)
        {
            var system = _disabledSystemList[i];
            if (!AreDependenciesSatisfied(system, entityManager))
            {
                i++;
                continue;
            }

            _disabledSystemList.RemoveAt(i);
            _activatedSystemList.Add(system);
            LogCat.Info($"Activate system: {GetSystemName(system)}");
            system.OnActivateStatusChanged(true);
        }

        for (var i = 0; i < _activatedSystemList.Count;)
        {
            var system = _activatedSystemList[i];
            if (AreDependenciesSatisfied(system, entityManager))
            {
                i++;
                continue;
            }

            _activatedSystemList.RemoveAt(i);
            _disabledSystemList.Add(system);
            LogCat.Info($"Disable system: {GetSystemName(system)}");
            system.OnActivateStatusChanged(false);
        }
    }

    /// <summary>
    /// <para>Whether every dependent component type of the system exists in the world.</para>
    /// <para>系统依赖的每种组件是否都存在于世界中。</para>
    /// </summary>
    private static bool AreDependenciesSatisfied(ISystem system, EntityManager entityManager)
    {
        foreach (var componentType in system.DependentComponents)
        {
            if (entityManager.CountComponents(componentType) == 0)
            {
                return false;
            }
        }

        return true;
    }

    private static string GetSystemName(ISystem system)
    {
        var type = system.GetType();
        return type.FullName ?? type.Name;
    }
}