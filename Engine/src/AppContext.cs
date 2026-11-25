using GlimmerWorks.Engine.Cache;
using GlimmerWorks.Engine.TemplateExpander;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Engine;

public class AppContext
{
    public VirtualFileSystem VirtualFileSystem { get; } = new();

    public DataRepository DataRepository { get; } = new();

    public TomlTemplateExpander TomlTemplateExpander { get; } = new();

    public CacheManager CacheManager { get; } = new();

    public GameTick TickTask { get; } = new();

    public EngineConfig EngineConfig { get; protected init; } = new();

    protected AppContext()
    {
        VirtualFileSystem.AddFileSystemProvider(new LocalFileSystemProvider(System.AppContext.BaseDirectory));
    }
}