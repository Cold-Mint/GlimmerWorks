using System.Reflection;
using System.Text;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.engine.Vfs;
using AppContext = GlimmerWorks.Engine.AppContext;

namespace GlimmerWorks.engine.templateExpander;

public class TomlTemplateExpander
{
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

    /// <summary>
    /// <para>AddLine</para>
    /// <para>添加一行数据</para>
    /// </summary>
    /// <param name="stringBuilder"></param>
    /// <param name="line"></param>
    private void AddLine(StringBuilder stringBuilder, string line)
    {
        if (stringBuilder.Length == 0)
        {
            stringBuilder.Append(line);
        }
        else
        {
            stringBuilder.AppendLine(line);
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
        Stack<VariableMap> variableMapStack = new();
        Stack<bool> conditionStack = new();
        variableMapStack.Push(new VariableMap());
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
            if (line.StartsWith("#@"))
            {
                string commandLine = line.Substring(2);
                int openParen = commandLine.IndexOf('(');
                if (openParen < 0)
                {
                    AddLine(stringBuilder, line);
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

                var argsArray = argsStr.Split('\n');
                buffer.Clear();
                command.Execute(appContext, variableMapStack, argsArray, buffer, conditionStack, packId);
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
                if (conditionStack.Count == 0)
                {
                    AddLine(stringBuilder, line);
                }
                else
                {
                    if (conditionStack.Peek())
                    {
                        AddLine(stringBuilder, line);
                    }
                }
            }
        }

        LogCat.Info(stringBuilder.ToString());
        return stringBuilder.ToString();
    }
}