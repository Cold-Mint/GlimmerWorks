using ArchEntity = Arch.Core.Entity;

namespace GlimmerWorks.Engine.World.Ecs;

/// <summary>
/// <para>Entity</para>
/// <para>轻量实体句柄，包装 Arch 的实体标识，避免把第三方类型暴露到引擎 API 中。</para>
/// </summary>
/// <remarks>
/// <para>默认值不是有效实体；有效句柄只能由 <see cref="GlimmerWorks.Engine.World.EntityManager"/> 创建。</para>
/// <para>A default value is not a valid entity. Valid handles are created only by the EntityManager.</para>
/// </remarks>
public readonly struct Entity : IEntity, IEquatable<Entity>
{
    internal Entity(ArchEntity handle)
    {
        Handle = handle;
    }

    /// <summary>
    /// <para>Arch 的原始实体句柄，仅供 <see cref="GlimmerWorks.Engine.World.EntityManager"/> 使用。</para>
    /// </summary>
    internal ArchEntity Handle { get; }

    /// <inheritdoc />
    public int Id => Handle.Id;

    /// <inheritdoc />
    public int Version => Handle.Version;

    public bool Equals(Entity other)
    {
        return Handle.Id == other.Handle.Id &&
               Handle.WorldId == other.Handle.WorldId &&
               Handle.Version == other.Handle.Version;
    }

    public override bool Equals(object? obj)
    {
        return obj is Entity other && Equals(other);
    }

    public override int GetHashCode()
    {
        return HashCode.Combine(Handle.Id, Handle.WorldId, Handle.Version);
    }

    public static bool operator ==(Entity left, Entity right) => left.Equals(right);

    public static bool operator !=(Entity left, Entity right) => !left.Equals(right);

    public override string ToString() => $"Entity({Handle.Id}:{Handle.Version})";
}
