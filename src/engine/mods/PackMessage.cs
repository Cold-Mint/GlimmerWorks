using GlimmerWorks.Engine.Log;
using GlimmerWorks.engine.Vfs;

namespace GlimmerWorks.Engine.Mods;

public sealed class PackMessage(string message)
{
    public string Message => message;

    public VirtualPath? Path { get; set; }

    public LogLevel Level { get; set; } = LogLevel.Info;
}