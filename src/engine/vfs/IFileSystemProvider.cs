namespace GlimmerWorks.engine.Vfs;

public interface IFileSystemProvider
{
    /// <summary>
    /// <para>VirtualPathToRealPath</para>
    /// <para>虚拟文件路径转实际路径</para>
    /// </summary>
    /// <param name="virtualPath"></param>
    /// <returns></returns>
    RealPath? VirtualPathToRealPath(VirtualPath virtualPath);

    List<FileEntry>? List(VirtualPath path, string? searchPattern = null, bool recurseSubdirectories = false);

    bool DirectoryExists(VirtualPath path);

    bool FileExists(VirtualPath path);

    string? ReadAllText(VirtualPath path);
    Stream? OpenRead(VirtualPath path);
}