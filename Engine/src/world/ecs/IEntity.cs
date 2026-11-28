namespace GlimmerWorks.Engine.World.Ecs;

/// <summary>
/// <para>IEntity</para>
/// <para>实体句柄。实体本身只保存身份信息，所有状态都放在组件里。</para>
/// </summary>
/// <remarks>
/// <para>Use <see cref="GlimmerWorks.Engine.World.EntityManager"/> to create entities and change their components.</para>
/// </remarks>
public interface IEntity
{
    /// <summary>
    /// <para>Entity id inside the owning world. Ids are reused after an entity is destroyed.</para>
    /// <para>实体在所属世界内的 id，实体销毁后该 id 会被回收复用。</para>
    /// </summary>
    int Id { get; }

    /// <summary>
    /// <para>Handle version. It changes when an id is reused, so a stale handle can be detected.</para>
    /// <para>句柄版本号。id 被复用时会变化，用于识别已经失效的实体句柄。</para>
    /// </summary>
    int Version { get; }
}
