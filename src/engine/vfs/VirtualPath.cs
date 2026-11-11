namespace GlimmerWorks.engine.Vfs;

public class VirtualPath(params string[] path)
{
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