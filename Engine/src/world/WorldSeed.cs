using System.Globalization;

namespace GlimmerWorks.Engine.World;

/// <summary>
/// <para>WorldSeed</para>
/// <para>种子文本解析：数字文本按整数解析，否则取 64 位哈希的低 32 位（有符号）。</para>
/// </summary>
public static class WorldSeed
{
    public static long Parse(string? text)
    {
        var value = text?.Trim() ?? string.Empty;
        if (long.TryParse(value, NumberStyles.Integer, CultureInfo.InvariantCulture, out var parsed))
        {
            return parsed;
        }

        var hash = Hash64(value);
        return unchecked((int)(uint)(hash & 0xFFFFFFFFUL));
    }

    public static ulong Hash64(string value)
    {
        const ulong offset = 14695981039346656037UL;
        const ulong prime = 1099511628211UL;
        var hash = offset;
        foreach (var c in value)
        {
            hash ^= c;
            hash *= prime;
        }

        return hash;
    }
}
