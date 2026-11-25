namespace GlimmerWorks.Engine.Log;

/// <summary>
/// Log level; smaller values are more verbose.
/// 日志级别，值越小越详细。
/// </summary>
[Flags]
public enum LogLevels
{
    Debug = 1 << 0,
    Info = 1 << 1,
    Warn = 1 << 2,
    Error = 1 << 3
}

#pragma warning disable RS0030
public static class LogCat
{
    /// <summary>
    /// <para>LogLevelMask</para>
    /// <para>日志等级掩码</para>
    /// </summary>
    public static LogLevels LogLevelMask { get; set; } =
        LogLevels.Debug | LogLevels.Info | LogLevels.Warn | LogLevels.Error;

    private static bool FirstRedirectedOutput { get; set; } = true;

    public static void Debug(string text) => Write(LogLevels.Debug, text);

    public static void Info(string text) => Write(LogLevels.Info, text);

    public static void Warn(string text) => Write(LogLevels.Warn, text);

    public static void Error(string text) => Write(LogLevels.Error, text);

    private static string CreateLine(LogLevels level, string text)
    {
        return $"[{DateTime.Now:HH:mm:ss.fff}][{level}] {text}";
    }

    private static void Write(LogLevels level, string text)
    {
        if ((level & LogLevelMask) == 0)
        {
            return;
        }

        if (Console.IsOutputRedirected)
        {
            //If the output is redirected, then only plain text will be output.
            //如果输出被重定向那么仅输出纯文本。
            if (FirstRedirectedOutput)
            {
                FirstRedirectedOutput = false;
                Console.WriteLine(CreateLine(LogLevels.Info,
                    "It was detected that console redirection is enabled, and log highlighting has been disabled."));
            }

            Console.WriteLine(CreateLine(level, text));
            return;
        }

        var previous = Console.ForegroundColor;
        Console.ForegroundColor = GetConsoleColor(level);
        Console.WriteLine(CreateLine(level, text));
        Console.ForegroundColor = previous;
    }

    private static ConsoleColor GetConsoleColor(LogLevels level) => level switch
    {
        LogLevels.Debug => ConsoleColor.Gray,
        LogLevels.Info => ConsoleColor.Cyan,
        LogLevels.Warn => ConsoleColor.Yellow,
        LogLevels.Error => ConsoleColor.Red,
        _ => ConsoleColor.White,
    };
}
#pragma warning restore RS0030