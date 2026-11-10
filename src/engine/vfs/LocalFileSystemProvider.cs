namespace GlimmerWorks.engine.Vfs;

public class LocalFileSystemProvider(string rootPath) : IFileSystemProvider
{
    private readonly RealPath _rootFullPath = new(Path.GetFullPath(rootPath));

    public RealPath? VirtualPathToRealPath(VirtualPath virtualPath)
    {
        var candidate = Path.Combine(_rootFullPath.Path, virtualPath.Path);
        return candidate.StartsWith(_rootFullPath.Path) ? new RealPath(candidate) : null;
    }

    public List<FileEntry>? List(VirtualPath path, string? searchPattern = null, bool recurseSubdirectories = false)
    {
        var realPath = VirtualPathToRealPath(path);
        if (realPath == null)
        {
            return null;
        }

        var directoryInfo = new DirectoryInfo(realPath.Path);
        if (!directoryInfo.Exists)
        {
            return null;
        }

        var enumerationOptions = new EnumerationOptions
        {
            RecurseSubdirectories = recurseSubdirectories,
            IgnoreInaccessible = true
        };
        var fileSystemInfos =
            directoryInfo.EnumerateFileSystemInfos(searchPattern ?? "*", enumerationOptions);
        return
        [
            .. fileSystemInfos.Select(fileSystemInfo => FileEntry.FromFileSystemInfo(_rootFullPath, fileSystemInfo))
        ];
    }

    public bool DirectoryExists(VirtualPath path)
    {
        var realPath = VirtualPathToRealPath(path);
        return realPath != null && new DirectoryInfo(realPath.Path).Exists;
    }

    public bool FileExists(VirtualPath path)
    {
        var realPath = VirtualPathToRealPath(path);
        return realPath != null && new FileInfo(realPath.Path).Exists;
    }


    public string? ReadAllText(VirtualPath path)
    {
        var realPath = VirtualPathToRealPath(path);
        return realPath == null ? null : File.ReadAllText(realPath.Path);
    }

    public Stream? OpenRead(VirtualPath path)
    {
        var realPath = VirtualPathToRealPath(path);
        return realPath == null ? null : File.OpenRead(realPath.Path);
    }
}