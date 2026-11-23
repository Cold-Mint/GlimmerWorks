using System.Text;
using JetBrains.Annotations;

namespace GlimmerWorks.Engine.TemplateExpander.Command;

[UsedImplicitly]
public class UnSetTemplateCommand : ITemplateCommand
{
    public void Execute(AppContext appContext, Stack<VariableMap> variableMapStack, string[] args, StringBuilder buffer,
        Stack<bool> conditionStack,
        string requesterPackId)
    {
        if (variableMapStack.Count <= 0)
        {
            return;
        }

        var variableMap = variableMapStack.Peek();
        foreach (var arg in args)
        {
            variableMap.RemoveKey(arg);
        }
    }

    public string GetCommandName()
    {
        return "unset";
    }

    public bool ShouldExecuteWhenInactive()
    {
        return false;
    }
}