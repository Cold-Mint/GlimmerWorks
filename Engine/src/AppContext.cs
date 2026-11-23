using GlimmerWorks.Engine.TemplateExpander;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Engine;

public class AppContext
{
    public VirtualFileSystem VirtualFileSystem { get; } = new();

    public DataRepository DataRepository { get; init; } = new();
    
    public TomlTemplateExpander TomlTemplateExpander { get; init; } = new();

    protected AppContext()
    {
        VirtualFileSystem.AddFileSystemProvider(new LocalFileSystemProvider(System.AppContext.BaseDirectory));
    }
}