using System.Globalization;

namespace GlimmerWorks.Engine.Log;

/// <summary>
/// Unified logging utility. Method arguments are localization keys, translated via <see cref="LocalizationManager"/> before output.
/// 统一的日志输出工具。方法的入参是本地化 key，输出前会通过 <see cref="LocalizationManager"/> 翻译。
/// </summary>
public class LogCat
{
    /// <summary>
    /// Minimum output level; logs below this level are ignored.
    /// 最低输出级别，低于此级别的日志会被忽略。
    /// </summary>
    private LogLevel MinLevel { get; set; } = LogLevel.Info;

    /// <summary>
    /// Localization manager used to translate log keys. When not injected, logs are emitted as the key itself.
    /// 用于翻译日志 key 的本地化管理器。未注入时日志按 key 原样输出。
    /// </summary>
    public LocalizationManager? Localization { get; set; }

    public void Debug(string key) => Write(LogLevel.Debug, key, null, null);

    public void Debug(string key, params object?[] args) => Write(LogLevel.Debug, key, null, args);

    public void Info(string key) => Write(LogLevel.Info, key, null, null);

    public void Info(string key, params object?[] args) => Write(LogLevel.Info, key, null, args);

    public void Warn(string key) => Write(LogLevel.Warn, key, null, null);

    public void Warn(string key, params object?[] args) => Write(LogLevel.Warn, key, null, args);

    public void Error(string key) => Write(LogLevel.Error, key, null, null);

    public void Error(string key, params object?[] args) => Write(LogLevel.Error, key, null, args);

    public void Error(string key, Exception ex, params object?[] args) => Write(LogLevel.Error, key, ex, args);

    private void Write(LogLevel level, string key, Exception? ex, object?[]? args)
    {
        if (level < MinLevel)
            return;

        var text = Resolve(key, args);
        if (ex is not null)
            text = $"{text}：{ex.Message}";

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

    /// <summary>
    /// Translates the key and, when arguments are present, formats them via {0}, {1}… placeholders.
    /// 翻译 key，并在有参数时按 {0}、{1}… 占位符格式化。
    /// </summary>
    private string Resolve(string key, object?[]? args)
    {
        var text = Localization?.Get(key) ?? key;
        if (args is { Length: > 0 })
            text = string.Format(CultureInfo.CurrentCulture, text, args);

        return text;
    }
}