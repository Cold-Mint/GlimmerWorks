using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Engine.Mods;

public sealed class PackMessage(string message)
{
    public string Message => message;

    public VirtualPath? Path { get; init; }

    public LogLevel Level { get; init; } = LogLevel.Info;
}