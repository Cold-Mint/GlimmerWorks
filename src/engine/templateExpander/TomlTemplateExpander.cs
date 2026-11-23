using System.Reflection;
using System.Text;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Engine.TemplateExpander;

public class TomlTemplateExpander
{
    private const int MaxTemplateInsertDepth = 64;

    private readonly Dictionary<string, ITemplateCommand> _commandMap = new();

    public TomlTemplateExpander()
    {
        var interfaceType = typeof(ITemplateCommand);
        var assembly = Assembly.GetExecutingAssembly();

        var commandTypes = assembly.GetTypes()
            .Where(t =>
                t is { IsInterface: false, IsAbstract: false }
                && interfaceType.IsAssignableFrom(t));

        foreach (var type in commandTypes)
        {
            var cmd = (ITemplateCommand)Activator.CreateInstance(type)!;
            var name = cmd.GetCommandName();
            if (!_commandMap.TryAdd(name, cmd))
            {
                LogCat.Error(
                    $"Template command has the same name: {cmd.GetCommandName()}, type: {type.FullName ?? type.Name}");
                continue;
            }

            LogCat.Info($"Register template command: {cmd.GetCommandName()}");
        }
    }

    public string Expand(AppContext appContext, VirtualPath virtualPath, string packId)
    {
        var virtualFileSystem = appContext.VirtualFileSystem;
        var text = virtualFileSystem.ReadAllText(virtualPath);
        if (string.IsNullOrWhiteSpace(text))
        {
            return string.Empty;
        }

        StringBuilder stringBuilder = new();
        var firstOutputLine = true;
        Stack<VariableMap> variableMapStack = new();
        variableMapStack.Push(new VariableMap());
        var warnedVariables = new HashSet<string>(StringComparer.Ordinal);

        Stack<bool> conditionStack = new();
        var activeTemplates = new HashSet<string>(StringComparer.Ordinal);
        var templateInsertStack = new Stack<string>();
        var skippedTemplateInsertDepth = 0;
        LinkedList<string> lines = new();
        var stringArray = text.Split('\n');
        foreach (var line in stringArray)
        {
            lines.AddLast(line);
        }

        StringBuilder buffer = new();
        while (lines.Count > 0)
        {
            var first = lines.First;
            if (first == null)
            {
                break;
            }

            lines.RemoveFirst();
            var line = first.Value;
            if (line.EndsWith('\r'))
            {
                line = line[..^1];
            }

            if (TemplateInsertMarker.TryParse(line, out var isInsertBegin, out var templateKey))
            {
                if (skippedTemplateInsertDepth > 0)
                {
                    if (isInsertBegin)
                    {
                        skippedTemplateInsertDepth++;
                    }
                    else
                    {
                        skippedTemplateInsertDepth--;
                    }

                    continue;
                }

                if (isInsertBegin)
                {
                    if (activeTemplates.Contains(templateKey))
                    {
                        LogCat.Error(
                            $"Recursive template insertion detected: {templateKey} (file: {virtualPath.Path})");
                        skippedTemplateInsertDepth = 1;
                        continue;
                    }

                    if (templateInsertStack.Count >= MaxTemplateInsertDepth)
                    {
                        LogCat.Error(
                            $"Template insertion depth exceeded {MaxTemplateInsertDepth}: {templateKey} (file: {virtualPath.Path})");
                        skippedTemplateInsertDepth = 1;
                        continue;
                    }

                    activeTemplates.Add(templateKey);
                    templateInsertStack.Push(templateKey);
                }
                else
                {
                    if (templateInsertStack.Count > 0 && templateInsertStack.Peek() == templateKey)
                    {
                        templateInsertStack.Pop();
                    }
                    else
                    {
                        LogCat.Error(
                            $"Unbalanced template insertion marker: {templateKey} (file: {virtualPath.Path})");
                    }

                    activeTemplates.Remove(templateKey);
                }

                continue;
            }

            if (skippedTemplateInsertDepth > 0)
            {
                continue;
            }

            if (line.StartsWith("#@"))
            {
                string commandLine = line.Substring(2);
                int openParen = commandLine.IndexOf('(');
                if (openParen < 0)
                {
                    AddLine(stringBuilder, ref firstOutputLine, line, variableMapStack.Peek(), warnedVariables,
                        virtualPath.Path);
                    continue;
                }

                string commandName = commandLine.Substring(0, openParen);
                if (!_commandMap.TryGetValue(commandName, out var command))
                {
                    LogCat.Error($"Unknown template command:{commandName}");
                    continue;
                }

                int closeParen = commandLine.LastIndexOf(')');
                string argsStr;
                if (closeParen < 0)
                {
                    argsStr = commandLine.Substring(openParen + 1);
                }
                else
                {
                    argsStr = commandLine.Substring(openParen + 1, closeParen - openParen - 1);
                }

                var argsArray = ParseArguments(argsStr);
                buffer.Clear();
                if (command.ShouldExecuteWhenInactive() || TemplateCommandHelper.IsActive(conditionStack))
                {
                    command.Execute(appContext, variableMapStack, argsArray, buffer, conditionStack, packId);
                }

                var bufferString = buffer.ToString();
                if (bufferString.Length > 0)
                {
                    var bufferArray = bufferString.Split('\n');
                    var bufferArrayLength = bufferArray.Length;
                    if (bufferArrayLength > 0)  
                    {
                        for (int i = bufferArrayLength - 1; i >= 0; i--)
                        {
                            lines.AddFirst(bufferArray[i]);
                        }
                    }
                }
            }
            else
            {
                if (conditionStack.Count == 0 || conditionStack.All(static condition => condition))
                {
                    AddLine(stringBuilder, ref firstOutputLine, line, variableMapStack.Peek(), warnedVariables,
                        virtualPath.Path);
                }
            }
        }

        LogCat.Info(stringBuilder.ToString());
        return stringBuilder.ToString();
    }

    private static void AddLine(StringBuilder stringBuilder, ref bool firstOutputLine, string line,
        VariableMap variableMap, HashSet<string> warnedVariables, string filePath)
    {
        var outputLine = SubstituteVariables(line, variableMap, warnedVariables, filePath);
        if (!firstOutputLine)
        {
            stringBuilder.Append('\n');
        }

        stringBuilder.Append(outputLine);
        firstOutputLine = false;
    }

    private static string SubstituteVariables(string line, VariableMap variableMap, HashSet<string> warnedVariables,
        string filePath)
    {
        var cursor = 0;
        StringBuilder? builder = null;

        while (cursor < line.Length)
        {
            var open = line.IndexOf('{', cursor);
            if (open < 0)
            {
                break;
            }

            var close = line.IndexOf('}', open + 1);
            if (close < 0)
            {
                break;
            }

            builder ??= new StringBuilder(line.Length);
            builder.Append(line, cursor, open - cursor);
            var key = line.Substring(open + 1, close - open - 1);
            var isValidKey = IsVariableKey(key);
            if (isValidKey && variableMap.GetValue(key) is { } value)
            {
                builder.Append(value);
            }
            else
            {
                builder.Append(line, open, close - open + 1);
                if (isValidKey && warnedVariables.Add(key))
                {
                    LogCat.Warn($"Unknown template variable: {{{key}}} (file: {filePath})");
                }
            }

            cursor = close + 1;
        }

        if (builder == null)
        {
            return line;
        }

        if (cursor < line.Length)
        {
            builder.Append(line, cursor, line.Length - cursor);
        }

        return builder.ToString();
    }

    private static bool IsVariableKey(string key)
    {
        if (key.Length == 0)
        {
            return false;
        }

        foreach (var c in key)
        {
            if (!char.IsAsciiLetterOrDigit(c) && c != '_')
            {
                return false;
            }
        }

        return true;
    }

    private static string[] ParseArguments(string argsStr)
    {
        if (string.IsNullOrWhiteSpace(argsStr))
        {
            return [];
        }

        List<string> args = [];
        var current = new StringBuilder();
        var inQuotes = false;
        foreach (var c in argsStr)
        {
            if (c == '"')
            {
                inQuotes = !inQuotes;
                current.Append(c);
                continue;
            }

            if (c == ',' && !inQuotes)
            {
                args.Add(NormalizeArgument(current.ToString()));
                current.Clear();
                continue;
            }

            current.Append(c);
        }

        args.Add(NormalizeArgument(current.ToString()));
        return [.. args];
    }

    private static string NormalizeArgument(string arg)
    {
        var trimmed = arg.Trim();
        if (trimmed.Length >= 2 && trimmed[0] == '"' && trimmed[^1] == '"')
        {
            return trimmed[1..^1];
        }

        return trimmed;
    }
}