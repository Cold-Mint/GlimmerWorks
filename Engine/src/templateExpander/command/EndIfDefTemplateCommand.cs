using System.Text;
using JetBrains.Annotations;

namespace GlimmerWorks.Engine.TemplateExpander.Command;

[UsedImplicitly]
public class EndIfDefTemplateCommand : ITemplateCommand
{
    public void Execute(AppContext appContext, Stack<VariableMap> variableMapStack, string[] args, StringBuilder buffer,
        Stack<bool> conditionStack,
        string requesterPackId)
    {
        if (conditionStack.Count > 0)
        {
            conditionStack.Pop();
        }
    }

    public string GetCommandName()
    {
        return "end_if_def";
    }

    public bool ShouldExecuteWhenInactive()
    {
        return true;
    }
}