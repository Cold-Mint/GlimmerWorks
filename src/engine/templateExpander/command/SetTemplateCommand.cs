using System.Text;
using JetBrains.Annotations;

namespace GlimmerWorks.Engine.TemplateExpander.Command;

[UsedImplicitly]
public class SetTemplateCommand : ITemplateCommand
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
        //If the number is odd, then skip the last element.
        //如果是奇数，那么跳过最后一个元素。
        for (var i = 0; i + 1 < args.Length; i += 2)
        {
            var key = args[i];
            var value = args[i + 1];
            variableMap.SetValue(key, value);
        }
    }

    public string GetCommandName()
    {
        return "set";
    }

    public bool ShouldExecuteWhenInactive()
    {
        return false;
    }
}