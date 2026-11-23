using System.Text;
using JetBrains.Annotations;

namespace GlimmerWorks.Engine.TemplateExpander.Command;

[UsedImplicitly]
public class ScopeTemplateCommand : ITemplateCommand
{
    public void Execute(AppContext appContext, Stack<VariableMap> variableMapStack, string[] args, StringBuilder buffer,
        Stack<bool> conditionStack,
        string requesterPackId)
    {
        if (args.Length == 0 || string.IsNullOrWhiteSpace(args[0]))
        {
            variableMapStack.Push(new VariableMap());
            return;
        }

        if (args[0] == "inherit" && variableMapStack.Count > 0)
        {
            variableMapStack.Push(variableMapStack.Pop().Copy());
        }
    }

    public string GetCommandName()
    {
        return "scope";
    }

    public bool ShouldExecuteWhenInactive()
    {
        return true;
    }
}