using System.Text;
using JetBrains.Annotations;

namespace GlimmerWorks.Engine.TemplateExpander.Command;

[UsedImplicitly]
public class EndScopeTemplateCommand : ITemplateCommand
{
    public void Execute(AppContext appContext, Stack<VariableMap> variableMapStack, string[] args, StringBuilder buffer,
        Stack<bool> conditionStack,
        string requesterPackId)
    {
        if (variableMapStack.Count > 1)
        {
            variableMapStack.Pop();
        }
    }

    public string GetCommandName()
    {
        return "end_scope";
    }

    public bool ShouldExecuteWhenInactive()
    {
        return true;
    }
}