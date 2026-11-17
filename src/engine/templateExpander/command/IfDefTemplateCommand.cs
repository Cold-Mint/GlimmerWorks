using System.Text;
using JetBrains.Annotations;
using AppContext = GlimmerWorks.Engine.AppContext;

namespace GlimmerWorks.engine.templateExpander.command;

[UsedImplicitly]
public class IfDefTemplateCommand : ITemplateCommand
{
    public void Execute(AppContext appContext, Stack<VariableMap> variableMapStack, string[] args, StringBuilder buffer,
        Stack<bool> conditionStack,
        string requesterPackId)
    {
        if (variableMapStack.Count > 0)
        {
            var top = variableMapStack.Peek();
            var result = true;
            for (var i = 0; i < args.Length; i++)
            {
                if (top.ContainsKey(args[i]))
                {
                    continue;
                }
                result = false;
                break;
            }
            conditionStack.Push(result);
        }
    }

    public string GetCommandName()
    {
        return "if_def";
    }
}