namespace GlimmerWorks.engine.templateExpander;

/// <summary>
/// <para>Internal markers used to track nested template insertions.</para>
/// <para>用于跟踪嵌套模板插入的内部标记。</para>
/// </summary>
internal static class TemplateInsertMarker
{
    private const string Prefix = "\0template_insert:";
    private const string BeginPrefix = Prefix + "begin:";
    private const string EndPrefix = Prefix + "end:";

    /// <summary>
    /// <para>Create a begin marker for the specified template insertion.</para>
    /// <para>为指定的模板插入创建开始标记。</para>
    /// </summary>
    /// <param name="templateKey">Template key in the form <c>packId:resourceId</c>.</param>
    /// <returns>The begin marker line.</returns>
    public static string CreateBegin(string templateKey) => BeginPrefix + templateKey;

    /// <summary>
    /// <para>Create an end marker for the specified template insertion.</para>
    /// <para>为指定的模板插入创建结束标记。</para>
    /// </summary>
    /// <param name="templateKey">Template key in the form <c>packId:resourceId</c>.</param>
    /// <returns>The end marker line.</returns>
    public static string CreateEnd(string templateKey) => EndPrefix + templateKey;

    /// <summary>
    /// <para>Try to parse a line as a template insertion begin or end marker.</para>
    /// <para>尝试将一行解析为模板插入的开始或结束标记。</para>
    /// </summary>
    /// <param name="line">Line to inspect.</param>
    /// <param name="isBegin">
    /// <para><c>true</c> when the line is a begin marker; <c>false</c> when it is an end marker.</para>
    /// <para>如果该行是开始标记则为 <c>true</c>；如果是结束标记则为 <c>false</c>。</para>
    /// </param>
    /// <param name="templateKey">Parsed template key, or an empty string when parsing fails.</param>
    /// <returns><c>true</c> when the line is a template insertion marker; otherwise <c>false</c>.</returns>
    public static bool TryParse(string line, out bool isBegin, out string templateKey)
    {
        isBegin = false;
        templateKey = string.Empty;

        if (line.StartsWith(BeginPrefix, StringComparison.Ordinal))
        {
            isBegin = true;
            templateKey = line[BeginPrefix.Length..];
            return true;
        }

        if (line.StartsWith(EndPrefix, StringComparison.Ordinal))
        {
            templateKey = line[EndPrefix.Length..];
            return true;
        }

        return false;
    }
}
