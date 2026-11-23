namespace GlimmerWorks.Engine.TemplateExpander;

public class VariableMap
{
    private readonly Dictionary<string, string> _variableDictionary = new();

    /// <summary>
    /// <para>GetValue</para>
    /// <para>获取值</para>
    /// </summary>
    /// <param name="key"></param>
    /// <returns></returns>
    public string? GetValue(string key)
    {
        return _variableDictionary.GetValueOrDefault(key);
    }

    /// <summary>
    /// <para>SetValue</para>
    /// <para>设置值</para>
    /// </summary>
    /// <param name="key"></param>
    /// <param name="value"></param>
    public void SetValue(string key, string value)
    {
        _variableDictionary[key] = value;
    }

    /// <summary>
    /// <para>RemoveKey</para>
    /// <para>移除Key</para>
    /// </summary>
    /// <param name="key"></param>
    public void RemoveKey(string key)
    {
        _variableDictionary.Remove(key);
    }

    /// <summary>
    /// <para>Copy</para>
    /// <para>复制一份</para>
    /// </summary>
    /// <returns></returns>
    public VariableMap Copy()
    {
        VariableMap newVariableMap = new();
        foreach (var keyValuePair in _variableDictionary)
        {
            newVariableMap.SetValue(keyValuePair.Key, keyValuePair.Value);
        }
        return newVariableMap;
    }

    /// <summary>
    /// <para>Count</para>
    /// <para>数量</para>
    /// </summary>
    public int Count => _variableDictionary.Count;

    /// <summary>
    /// <para>ContainsKey</para>
    /// <para>是否包含某个key</para>
    /// </summary>
    /// <param name="key"></param>
    /// <returns></returns>
    public bool ContainsKey(string key)
    {
        return _variableDictionary.ContainsKey(key);
    }
}