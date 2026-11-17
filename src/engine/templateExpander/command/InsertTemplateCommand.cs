using System.Text;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Data;
using JetBrains.Annotations;
using AppContext = GlimmerWorks.Engine.AppContext;

namespace GlimmerWorks.engine.templateExpander.command;

[UsedImplicitly]
public class InsertTemplateCommand : ITemplateCommand
{
    public void Execute(AppContext appContext, Stack<VariableMap> variableMapStack, string[] args, StringBuilder buffer,
        Stack<bool> conditionStack,
        string requesterPackId)
    {
        if (string.IsNullOrWhiteSpace(requesterPackId))
        {
            return;
        }

        foreach (var arg in args)
        {
            var resourceRef = ResourceRef.FromString(arg, requesterPackId);
            if (resourceRef == null)
            {
                continue;
            }

            var templateResources = appContext.DataRepository.Get<TemplateResources>(resourceRef);
            if (templateResources == null)
            {
                continue;
            }

            buffer.AppendLine(templateResources.Content);
        }
    }

    public string GetCommandName()
    {
        return "insert";
    }
}