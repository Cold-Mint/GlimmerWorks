namespace GlimmerWorks.Engine.TemplateExpander;

/// <summary>
/// <para>Helpers shared by template commands.</para>
/// <para>模板命令共用的辅助方法。</para>
/// </summary>
internal static class TemplateCommandHelper
{
    /// <summary>
    /// <para>Return true only when every enclosing condition is true.</para>
    /// <para>仅当所有外层条件都为 true 时返回 true。</para>
    /// </summary>
    public static bool IsActive(Stack<bool> conditionStack)
    {
        foreach (var condition in conditionStack)
        {
            if (!condition)
            {
                return false;
            }
        }

        return true;
    }
}
