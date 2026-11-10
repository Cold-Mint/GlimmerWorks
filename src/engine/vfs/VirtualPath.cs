namespace GlimmerWorks.engine.Vfs;

public class VirtualPath(params string[] path)
{
    public string Path { get; } = System.IO.Path.Combine(path);

    public VirtualPath Combine(params string[] paths)
    {
        var list = new List<string> { Path };
        list.AddRange(paths);
        return new VirtualPath([.. list]);
    }
}