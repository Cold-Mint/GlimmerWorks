using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Engine.Mods;

public sealed class PackMessage(string name, params object[] args)
{
    public string Name => name;

    public object[] Args => args;

    public VirtualPath? Path { get; init; }

    public LogLevel Level { get; init; } = LogLevel.Info;
}