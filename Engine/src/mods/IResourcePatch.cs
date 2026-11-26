namespace GlimmerWorks.Engine.Mods;

/// <summary>
/// <para>IResourcePatch</para>
/// <para>资源补丁</para>
/// </summary>
public interface IResourcePatch
{
    void ApplyPatch(DataRepository dataRepository);
}