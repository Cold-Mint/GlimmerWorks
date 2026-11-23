using System.Text;

namespace GlimmerWorks.Engine.TemplateExpander;


public interface ITemplateCommand
{
    public void Execute(AppContext appContext, Stack<VariableMap> variableMapStack, string[] args, StringBuilder buffer,
        Stack<bool> conditionStack, string requesterPackId);

    /// <summary>
    /// <para>GetCommandName</para>
    /// <para>命令名称</para>
    /// </summary>
    /// <returns></returns>
    public string GetCommandName();

    /// <summary>
    /// <para>Whether to execute this command when an enclosing condition is false.</para>
    /// <para>当外层条件为 false 时，该命令是否仍然执行。</para>
    /// </summary>
    /// <returns></returns>
    public bool ShouldExecuteWhenInactive();
}