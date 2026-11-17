namespace GlimmerWorks.engine;

using AppContext = GlimmerWorks.Engine.AppContext;

/// <summary>
/// <para>PackContext</para>
/// <para>包上下文</para>
///<para>The Toml template system relies on this context for template search.</para>
///<para>toml模板系统依赖此上下文进行模板查找。</para>
/// </summary>
public class PackContext
{
    public AppContext? AppContext { get; set; }

    public string? RequesterPackId { get; set; }
}