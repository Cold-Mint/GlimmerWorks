namespace GlimmerWorks.engine.Vfs;

public class FileEntry
{
    public bool IsDirectory { set; get; }
    public string Name { set; get; }
    public VirtualPath Path { set; get; }

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