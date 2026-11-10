using GlimmerWorks.engine.Vfs;
using Tomlyn;

namespace GlimmerWorks.Engine;

public class VirtualFileSystem
{
    private readonly List<IFileSystemProvider> _fileSystemProviders = [];

    public void AddFileSystemProvider(IFileSystemProvider provider) => _fileSystemProviders.Add(provider);

    public void RemoveFileSystemProvider(IFileSystemProvider provider) => _fileSystemProviders.Remove(provider);

    /// <summary>
    /// <para>List</para>
    /// <para>列出文件和文件夹</para>
    /// </summary>
    /// <param name="path"></param>
    /// <param name="searchPattern"></param>
    /// <param name="recurseSubdirectories"></param>
    /// <returns></returns>
    public List<FileEntry>? List(VirtualPath path, string? searchPattern = null, bool recurseSubdirectories = false) =>
        _fileSystemProviders
            .Select(fileSystemProvider => fileSystemProvider.List(path, searchPattern, recurseSubdirectories))
            .Select(result => result ?? null).FirstOrDefault();

    /// <summary>
    /// <para>ReadAllText</para>
    /// <para>读取全部内容</para>
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    public string? ReadAllText(VirtualPath path) => _fileSystemProviders
        .Select(fileSystemProvider => fileSystemProvider.ReadAllText(path))
        .OfType<string>()
        .FirstOrDefault();

    /// <summary>
    /// <para>OpenRead</para>
    /// <para>打开文件</para>
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    public Stream? OpenRead(VirtualPath path) => _fileSystemProviders
        .Select(fileSystemProvider => fileSystemProvider.OpenRead(path))
        .OfType<Stream>()
        .FirstOrDefault();


    /// <summary>
    /// <para>DirectoryExists</para>
    /// <para>目录是否存在</para>
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    public bool DirectoryExists(VirtualPath path) =>
        _fileSystemProviders.Any(fileSystemProvider => fileSystemProvider.DirectoryExists(path));

    /// <summary>
    /// <para>FileExists</para>
    /// <para>文件是否存在</para>
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    public bool FileExists(VirtualPath path) =>
        _fileSystemProviders.Any(fileSystemProvider => fileSystemProvider.FileExists(path));

    /// <summary>
    /// <para>ReadToml</para>
    /// <para>读取Toml</para>
    /// </summary>
    /// <param name="path"></param>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    public T? ReadToml<T>(VirtualPath path)
    {
        var text = ReadAllText(path);
        return text == null ? default : TomlSerializer.Deserialize<T>(text);
    }

    /// <summary>
    /// <para>ReadToml</para>
    /// <para>读取Toml</para>
    /// </summary>
    /// <param name="path"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public object? ReadToml(VirtualPath path, Type type)
    {
        var text = ReadAllText(path);
        return text == null ? null : TomlSerializer.Deserialize(text, type);
    }
}