namespace GlimmerWorks.Engine.Vfs;

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

    
    FileEntry? GetFileEntry(VirtualPath path);
    
    bool FileExists(VirtualPath path);

    string? ReadAllText(VirtualPath path);
    bool WriteAllText(VirtualPath path, string text);

    /// <summary>
    /// <para>ReadAllBytes</para>
    /// <para>读取全部字节</para>
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    byte[]? ReadAllBytes(VirtualPath path);

    /// <summary>
    /// <para>WriteAllBytes</para>
    /// <para>将字节写入到文件内</para>
    /// </summary>
    /// <param name="path"></param>
    /// <param name="bytes"></param>
    /// <returns></returns>
    bool WriteAllBytes(VirtualPath path, ReadOnlySpan<byte> bytes);
    
    

    Stream? OpenRead(VirtualPath path);
}