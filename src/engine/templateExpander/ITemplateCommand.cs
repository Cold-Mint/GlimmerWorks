using System.Text;

namespace GlimmerWorks.engine.templateExpander;

using AppContext = GlimmerWorks.Engine.AppContext;

public interface ITemplateCommand
{
    public void Execute(AppContext appContext, Stack<VariableMap> variableMapStack, string[] args, StringBuilder buffer,
        Stack<bool> conditionStack, string packId);

    /// <summary>
    /// <para>GetCommandName</para>
    /// <para>命令名称</para>
    /// </summary>
    /// <returns></returns>
    public string GetCommandName();
}