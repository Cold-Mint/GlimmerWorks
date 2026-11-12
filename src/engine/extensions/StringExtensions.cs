using System.Text.RegularExpressions;

namespace GlimmerWorks.engine.extensions;

public static class StringExtensions
{
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

    private static readonly Regex BoundaryRegex =
        new(@"(?<=[a-z0-9])([A-Z])", RegexOptions.Compiled);

    private static readonly Regex NonAlnumRegex =
        new(@"[^a-zA-Z0-9]+", RegexOptions.Compiled);

    public static string ToSnakeCase(this string str)
    {
        if (string.IsNullOrEmpty(str))
            return str;

        //Illegal characters → Underline
        //1.非法字符 → 下划线
        var s = NonAlnumRegex.Replace(str, "_");

        //2. Convexity boundary → Underline
        //2.驼峰边界 → 下划线
        s = BoundaryRegex.Replace(s, "_$1");

        //3. Lowercase + Folded __ + Remove leading and trailing _
        //3.小写 + 折叠 __ + 去前后 _
        s = s.ToLowerInvariant();
        s = Regex.Replace(s, "_+", "_");
        return s.Trim('_');
    }
}