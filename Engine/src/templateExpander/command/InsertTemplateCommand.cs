using System.Text;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;
using JetBrains.Annotations;

namespace GlimmerWorks.Engine.TemplateExpander.Command;

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

            var templateResources = appContext.DataRepository.Get<TemplateResource>(resourceRef);
            if (templateResources == null)
            {
                continue;
            }

            var content = templateResources.Content;
            if (content.EndsWith("\r\n"))
            {
                content = content[..^2];
            }
            else if (content.EndsWith('\n') || content.EndsWith('\r'))
            {
                content = content[..^1];
            }

            var templateKey = $"{resourceRef.TargetPackId}:{resourceRef.ResourceId}";
            if (buffer.Length > 0)
            {
                buffer.Append('\n');
            }

            buffer.Append(TemplateInsertMarker.CreateBegin(templateKey));
            buffer.Append('\n');
            if (content.Length > 0)
            {
                buffer.Append(content);
                buffer.Append('\n');
            }

            buffer.Append(TemplateInsertMarker.CreateEnd(templateKey));
        }
    }

    public string GetCommandName()
    {
        return "insert";
    }

    public bool ShouldExecuteWhenInactive()
    {
        return false;
    }
}