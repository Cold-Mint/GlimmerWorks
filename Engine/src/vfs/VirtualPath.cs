namespace GlimmerWorks.Engine.Vfs;

public class VirtualPath(params string[] path)
{
    public static readonly VirtualPath DataPackPath  = new("mods", "data_packs");
    public static readonly VirtualPath ResourcePackPath = new("mods", "resource_packs");
    
    public string Path { get; } = System.IO.Path.Combine(path);

    /// <summary>
    /// <para>GetRelativePathTo</para>
    /// <para>获取与另一个路径的相对路径</para>
    /// </summary>
    /// <param name="rootPath"></param>
    /// <returns></returns>
    public string GetRelativePath(VirtualPath rootPath)
    {
        return System.IO.Path.GetRelativePath(rootPath.Path, Path);
    }

    public VirtualPath Combine(params string[] paths)
    {
        var list = new List<string> { Path };
        list.AddRange(paths);
        return new VirtualPath([.. list]);
    }
}