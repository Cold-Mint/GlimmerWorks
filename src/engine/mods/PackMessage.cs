namespace GlimmerWorks.Engine.Mods;

public sealed class PackMessage(string message)
{
    public string Message => message;

    public PackMessageLevel Level => PackMessageLevel.Info;
}

public enum PackMessageLevel
{
    Info,
    Warning,
    Error,
}