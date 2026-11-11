using GlimmerWorks.Engine.Log;

namespace GlimmerWorks.Engine.Mods;

public sealed class PackMessage(string message)
{
    public string Message => message;

    public LogLevel Level => LogLevel.Info;
}

