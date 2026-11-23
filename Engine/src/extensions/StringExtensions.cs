using System.Text.RegularExpressions;

namespace GlimmerWorks.Engine.Extensions;

public static partial class StringExtensions
{
    [GeneratedRegex("(?<=[a-z0-9])([A-Z])")]
    private static partial Regex BoundaryRegex { get; }

    [GeneratedRegex("[^a-zA-Z0-9]+")]
    private static partial Regex NonAlphanumericRegex { get; }

    [GeneratedRegex("_+")] private static partial Regex RepeatedUnderscoreRegex { get; }

    /// <summary>
    /// <para>IsSnakeCase</para>
    /// <para>判断字符串是否为蛇形命名法</para>
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    public static bool IsSnakeCase(this string str)
    {
        if (string.IsNullOrEmpty(str))
        {
            return false;
        }

        if (str[0] == '_' || str[^1] == '_')
        {
            return false;
        }

        if (char.IsDigit(str[0]))
        {
            return false;
        }

        bool prevUnderscore = false;
        foreach (char c in str)
        {
            if (c == '_')
            {
                if (prevUnderscore)
                {
                    //Continuous Underline
                    //连续下划线
                    return false;
                }

                prevUnderscore = true;
            }
            else if (c is >= 'a' and <= 'z' or >= '0' and <= '9')
            {
                prevUnderscore = false;
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    public static string ToSnakeCase(this string str)
    {
        if (string.IsNullOrEmpty(str))
            return str;

        //Illegal characters → Underline
        //1.非法字符 → 下划线
        var s = NonAlphanumericRegex.Replace(str, "_");

        //2. Convexity boundary → Underline
        //2.驼峰边界 → 下划线
        s = BoundaryRegex.Replace(s, "_$1");

        //3. Lowercase + Folded __ + Remove leading and trailing _
        //3.小写 + 折叠 __ + 去前后 _
        s = s.ToLowerInvariant();
        s = RepeatedUnderscoreRegex.Replace(s, "_");
        return s.Trim('_');
    }
}