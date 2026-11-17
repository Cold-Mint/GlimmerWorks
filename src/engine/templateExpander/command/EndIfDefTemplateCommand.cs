using System.Text;
using JetBrains.Annotations;
using AppContext = GlimmerWorks.Engine.AppContext;

namespace GlimmerWorks.engine.templateExpander.command;

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
}