namespace GlimmerWorks.Engine.Log;

/// <summary>
/// Log level; smaller values are more verbose.
/// 日志级别，值越小越详细。
/// </summary>
public enum LogLevel
{
    Debug = 0,
    Info = 1,
    Warn = 2,
    Error = 3
}

/// <summary>
/// Unified logging utility. Method arguments are localization keys, translated via <see cref="LocalizationManager"/> before output.
/// 统一的日志输出工具。方法的入参是本地化 key，输出前会通过 <see cref="LocalizationManager"/> 翻译。
/// </summary>
#pragma warning disable RS0030
public static class LogCat
{
    /// <summary>
    /// Minimum output level; logs below this level are ignored.
    /// 最低输出级别，低于此级别的日志会被忽略。
    /// </summary>
    private static LogLevel MinLevel { get; set; } = LogLevel.Info;

    public static void Debug(string text) => Write(LogLevel.Debug, text);

    public static void Info(string text) => Write(LogLevel.Info, text);

    public static void Warn(string text) => Write(LogLevel.Warn, text);

    public static void Error(string text) => Write(LogLevel.Error, text);

    private static void Write(LogLevel level, string text)
    {
        if (level < MinLevel)
        {
            return;
        }

        var line = $"[{DateTime.Now:HH:mm:ss.fff}] [{level,-5}] {text}";
        if (Console.IsOutputRedirected)
        {
            Console.WriteLine(line);
            return;
        }

        var previous = Console.ForegroundColor;
        Console.ForegroundColor = level switch
        {
            LogLevel.Debug => ConsoleColor.Gray,
            LogLevel.Info => ConsoleColor.Cyan,
            LogLevel.Warn => ConsoleColor.Yellow,
            LogLevel.Error => ConsoleColor.Red,
            _ => ConsoleColor.White,
        };
        Console.WriteLine(line);
        Console.ForegroundColor = previous;
    }
}
#pragma warning restore RS0030