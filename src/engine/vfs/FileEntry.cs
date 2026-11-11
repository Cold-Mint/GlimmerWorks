namespace GlimmerWorks.engine.Vfs;

public class FileEntry
{
    public bool IsDirectory { set; get; }
    public string Name { set; get; }
    public VirtualPath Path { set; get; }

    /// <summary>
    /// <para>GetResourceType</para>
    /// <para>获取文件的资源类型</para>
    /// </summary>
    /// <returns></returns>
    public string? GetResourceType()
    {
        var span = System.IO.Path.GetFileName(Name.AsSpan());
        var lastDot = span.LastIndexOf('.');
        if (lastDot <= 0)
        {
            return null;
        }

        span = span[..lastDot];
        var prevDot = span.LastIndexOf('.');
        return prevDot < 0 ? null : span[(prevDot + 1)..].ToString();
    }

    /// <summary>
    /// <para>FromFileSystemInfo</para>
    /// <para>从FileSystemInfo创建</para>
    /// </summary>
    /// <param name="rootPath"></param>
    /// <param name="fileSystemInfo"></param>
    /// <returns></returns>
    public static FileEntry FromFileSystemInfo(RealPath rootPath, FileSystemInfo fileSystemInfo)
    {
        return new FileEntry
        {
            Name = fileSystemInfo.Name,
            IsDirectory = fileSystemInfo.Attributes.HasFlag(FileAttributes.Directory),
            Path = new VirtualPath(System.IO.Path.GetRelativePath(rootPath.Path, fileSystemInfo.FullName))
        };
    }
}