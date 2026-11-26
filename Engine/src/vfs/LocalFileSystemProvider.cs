namespace GlimmerWorks.Engine.Vfs;

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

    public bool Mkdir(VirtualPath path)
    {
        var realPath = VirtualPathToRealPath(path);
        if (realPath == null)
        {
            return false;
        }

        Directory.CreateDirectory(realPath.Path);
        return true;
    }

    public FileEntry? GetFileEntry(VirtualPath path)
    {
        var realPath = VirtualPathToRealPath(path);
        if (realPath == null)
        {
            return null;
        }

        return FileEntry.FromFileSystemInfo(_rootFullPath, new FileInfo(realPath.Path));
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

    public bool WriteAllText(VirtualPath path, string text)
    {
        var realPath = VirtualPathToRealPath(path);
        if (realPath == null)
        {
            return false;
        }

        File.WriteAllText(realPath.Path, text);
        return true;
    }


    public byte[]? ReadAllBytes(VirtualPath path)
    {
        var realPath = VirtualPathToRealPath(path);
        return realPath == null ? null : File.ReadAllBytes(realPath.Path);
    }

    public bool WriteAllBytes(VirtualPath path, ReadOnlySpan<byte> bytes)
    {
        var realPath = VirtualPathToRealPath(path);
        if (realPath == null)
        {
            return false;
        }

        using var fileStream = new FileStream(realPath.Path, FileMode.Create, FileAccess.Write, FileShare.None);
        fileStream.Write(bytes);
        return true;
    }


    public Stream? OpenRead(VirtualPath path)
    {
        var realPath = VirtualPathToRealPath(path);
        return realPath == null ? null : File.OpenRead(realPath.Path);
    }
}