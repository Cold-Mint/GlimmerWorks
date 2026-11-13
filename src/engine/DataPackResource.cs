namespace GlimmerWorks.engine;

[AttributeUsage(AttributeTargets.Class, Inherited = false)]
public sealed class DataPackResource(
    string resourceType,
    bool autoDeserialization = true,
    bool exportResourceReferences = true) : Attribute
{
    /// <summary>
    /// <para>ResourceType</para>
    /// <para>资源类型标识，对应 TOML 中的 resource_type</para>
    /// </summary>
    public string ResourceType { get; } = resourceType.Length == 0
        ? throw new ArgumentException("resourceType cannot be empty")
        : resourceType;

    /// <summary>
    /// <para>Whether to automatically deserialize from the toml file</para>
    /// <para>是否从toml文件自动反序列化</para>
    /// </summary>
    public bool AutoDeserialization => autoDeserialization;
    
    /// <summary>
    /// <para>Whether to export it to the resource reference</para>
    /// <para>是否将其导出到资源引用</para>
    /// </summary>
    public bool ExportResourceReferences => exportResourceReferences;
}