using System.Reflection;
using GlimmerWorks.Engine.Extensions;
using GlimmerWorks.Engine.Log;

namespace GlimmerWorks.Game.Graphics.Uniform;

/// <summary>
/// <para>
/// Collects public static members marked with <see cref="UniformValueAttribute"/>
/// from the game assembly via reflection.
/// </para>
/// <para>
/// 通过反射收集游戏程序集内标注了 <see cref="UniformValueAttribute"/> 的公开静态成员。
/// </para>
/// </summary>
public sealed class RuntimeUniformManager
{
    private const BindingFlags StaticMemberFlags =
        BindingFlags.Public | BindingFlags.Static | BindingFlags.DeclaredOnly;

    /// <summary>
    /// <para>The getters of the attributed static members, keyed by runtime key.</para>
    /// <para>被标注的静态成员的读取器，以运行时键索引。</para>
    /// </summary>
    private readonly Dictionary<string, Func<object?>> _valueGetters = [];

    private readonly HashSet<string> _reportedKeys = [];

    public RuntimeUniformManager()
    {
        var assembly = Assembly.GetExecutingAssembly();
        foreach (var type in assembly.GetTypes())
        {
            RegisterUniformValues(type);
        }
    }

    /// <summary>
    /// <para>Try to resolve a runtime uniform value.</para>
    /// <para>尝试解析运行时 uniform 值。</para>
    /// </summary>
    /// <param name="key"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    public bool TryGetValue(string key, out UniformValue value)
    {
        if (_valueGetters.TryGetValue(key, out var getter))
        {
            if (UniformValue.TryFromObject(getter(), out value))
            {
                return true;
            }

            if (_reportedKeys.Add(key))
            {
                LogCat.Error($"Runtime uniform value '{key}' has an unsupported type.");
            }

            value = default;
            return false;
        }

        if (_reportedKeys.Add(key))
        {
            LogCat.Warn($"No runtime uniform value found for key: {key}");
        }

        value = default;
        return false;
    }

    /// <summary>
    /// <para>Register public static fields and properties marked with <see cref="UniformValueAttribute"/>.</para>
    /// <para>注册标注了 <see cref="UniformValueAttribute"/> 的公开静态字段与属性。</para>
    /// </summary>
    private void RegisterUniformValues(Type type)
    {
        foreach (var field in type.GetFields(StaticMemberFlags))
        {
            var attribute = field.GetCustomAttribute<UniformValueAttribute>();
            if (attribute == null)
            {
                continue;
            }

            var capturedField = field;
            RegisterValue(type, field.Name, attribute, () => capturedField.GetValue(null));
        }

        foreach (var property in type.GetProperties(StaticMemberFlags))
        {
            var attribute = property.GetCustomAttribute<UniformValueAttribute>();
            if (attribute == null)
            {
                continue;
            }

            if (property.GetGetMethod(nonPublic: true) is not { IsStatic: true })
            {
                LogCat.Error(
                    $"Runtime uniform property {type.FullName}.{property.Name} must have a static getter.");
                continue;
            }

            var capturedProperty = property;
            RegisterValue(type, property.Name, attribute, () => capturedProperty.GetValue(null));
        }
    }

    private void RegisterValue(Type type, string memberName, UniformValueAttribute attribute,
        Func<object?> getter)
    {
        var key = string.IsNullOrWhiteSpace(attribute.Key)
            ? memberName.TrimStart('_').ToSnakeCase()
            : attribute.Key;
        if (!_valueGetters.TryAdd(key, getter))
        {
            LogCat.Error($"Duplicate runtime uniform key '{key}' from {type.FullName}.{memberName}.");
            return;
        }

        LogCat.Info($"Register runtime uniform value: {key} ({type.Name}.{memberName})");
    }
}
