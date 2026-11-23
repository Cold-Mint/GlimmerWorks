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

#pragma warning disable RS0030
public static class LogCat
{
    /// <summary>
    /// Minimum output level; logs below this level are ignored.
    /// 最低输出级别，低于此级别的日志会被忽略。
    /// </summary>
    private static LogLevel MinLevel { get; set; } = LogLevel.Info;
    
    private static bool FirstRedirectedOutput {get; set;} = true;

    public static void Debug(string text) => Write(LogLevel.Debug, text);

    public static void Info(string text) => Write(LogLevel.Info, text);

    public static void Warn(string text) => Write(LogLevel.Warn, text);

    public static void Error(string text) => Write(LogLevel.Error, text);

    private static string CreateLine(LogLevel level, string text)
    {
        return $"[{DateTime.Now:HH:mm:ss.fff}][{level}] {text}";
    }
    
    private static void Write(LogLevel level, string text)
    {
        if (level < MinLevel)
        {
            return;
        }
        if (Console.IsOutputRedirected)
        {
            //If the output is redirected, then only plain text will be output.
            //如果输出被重定向那么仅输出纯文本。
            if (FirstRedirectedOutput)
            {
                FirstRedirectedOutput  = false;
                Console.WriteLine(CreateLine(LogLevel.Info,"It was detected that console redirection is enabled, and log highlighting has been disabled."));
            }
            Console.WriteLine(CreateLine(level, text));
            return;
        }

        var previous = Console.ForegroundColor;
        Console.ForegroundColor = GetConsoleColor(level);
        Console.WriteLine(CreateLine(level, text));
        Console.ForegroundColor = previous;
    }

    private static ConsoleColor GetConsoleColor(LogLevel level) => level switch
    {
        LogLevel.Debug => ConsoleColor.Gray,
        LogLevel.Info => ConsoleColor.Cyan,
        LogLevel.Warn => ConsoleColor.Yellow,
        LogLevel.Error => ConsoleColor.Red,
        _ => ConsoleColor.White,
    };
}
#pragma warning restore RS0030