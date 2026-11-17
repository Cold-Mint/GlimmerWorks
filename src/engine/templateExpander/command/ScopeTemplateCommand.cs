using System.Text;
using JetBrains.Annotations;
using AppContext = GlimmerWorks.Engine.AppContext;

namespace GlimmerWorks.engine.templateExpander.command;

[UsedImplicitly]
public class ScopeTemplateCommand : ITemplateCommand
{
    public void Execute(AppContext appContext, Stack<VariableMap> variableMapStack, string[] args, StringBuilder buffer,
        Stack<bool> conditionStack,
        string requesterPackId)
    {
        if (args.Length == 0)
        {
            variableMapStack.Push(new VariableMap());
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
}