using GlimmerWorks.engine.Vfs;
using Microsoft.Extensions.Localization;
using Tomlyn.Model;

namespace GlimmerWorks.Engine;

/// <summary>
/// Localization Manager: Loads TOML language files by locale and provides text lookup with fallback.
/// 本地化管理器：按区域（locale）加载 TOML 语言文件，并提供带兜底的文本查询。
/// </summary>
public class LocalizationManager : IStringLocalizer
{
    private readonly List<LocalizedString> _localizedStringList = new();

    public IEnumerable<LocalizedString> GetAllStrings(bool includeParentCultures)
    {
        return _localizedStringList;
    }

    public LocalizedString this[string name] => Get(name);

    public LocalizedString this[string name, params object[] arguments] => GetFormat(name, arguments);


    /// <summary>
    /// Fallback locale directory name. When a key is missing in the target locale, fall back to this locale.
    /// 兜底语言目录名。当目标语言缺少某个 key 时回退到此语言。
    /// </summary>
    public const string DefaultLocale = "default";

    /// <summary>
    /// Active translation table. Reading the reference is an atomic operation and can be safely accessed without locking.
    /// 当前生效的翻译表。读取引用是原子操作，无需加锁即可安全访问。
    /// </summary>
    private readonly Dictionary<string, string> _strings = new(StringComparer.Ordinal);


    /// <summary>
    /// Gets the text; returns the key itself when not found, to expose untranslated items.
    /// 获取文本；找不到时返回 key 本身，便于暴露未翻译项。
    /// </summary>
    public LocalizedString Get(string name)
    {
        return new LocalizedString(name, _strings.GetValueOrDefault(name, name));
    }

    /// <summary>
    /// <para>Get text with formatting capabilities</para>
    /// <para>获取文本并带有格式化功能。</para>
    /// </summary>
    /// <param name="name"></param>
    /// <param name="args"></param>
    /// <returns></returns>
    public LocalizedString GetFormat(string name, params object[] args)
    {
        if (!_strings.TryGetValue(name, out var value))
        {
            return new LocalizedString(name, string.Format(name, args), true);
        }

        return new LocalizedString(name, string.Format(value, args));
    }

    public void Clear() => _strings.Clear();

    /// <summary>
    /// <para>Input the language and obtain the list of candidate languages.</para>
    /// <para>输入语言，获取语言的候选者列表。</para>
    /// </summary>
    /// <param name="language"></param>
    /// <returns></returns>
    public static List<string> GetCandidates(string language)
    {
        var candidates = new List<string>();
        if (string.IsNullOrEmpty(language))
        {
            candidates.Add(DefaultLocale);
            return candidates;
        }

        candidates.Add(language);
        var dash = language.IndexOf('-');
        if (dash <= 0)
        {
            return candidates;
        }

        candidates.Add(language.Replace('-', '_'));
        candidates.Add(language[..dash]);
        return candidates;
    }


    /// <summary>
    /// Loads all *.toml files under a locale directory, writing them into the target table with the file name as the key prefix.
    /// 加载某个语言目录下的所有 *.toml 文件，以文件名作为 key 前缀写入目标表。
    /// </summary>
    public bool LoadDirectory(
        VirtualFileSystem virtualFileSystem,
        VirtualPath directoryPath)
    {
        if (!virtualFileSystem.DirectoryExists(directoryPath))
        {
            return false;
        }

        var entries = virtualFileSystem.List(directoryPath, "*.toml");
        if (entries == null)
        {
            return false;
        }

        foreach (var entry in entries)
        {
            var category = Path.GetFileNameWithoutExtension(entry.Name);
            var table = virtualFileSystem.ReadToml<TomlTable>(entry.Path);
            if (table is not null)
            {
                Flatten(table, category, _strings, _localizedStringList);
            }
        }

        return true;
    }

    /// <summary>
    /// Flattens a TOML table into "prefix.key" key-value pairs, expanding nested tables recursively.
    /// 将 TOML 表扁平化为 "前缀.键" 形式的键值对，嵌套表递归展开。
    /// </summary>
    private static void Flatten(
        TomlTable table,
        string prefix,
        IDictionary<string, string> target, List<LocalizedString> localizedStringList)
    {
        foreach (var pair in table)
        {
            var fullKey = string.IsNullOrEmpty(prefix) ? pair.Key : $"{prefix}.{pair.Key}";
            if (pair.Value is TomlTable nested)
            {
                Flatten(nested, fullKey, target, localizedStringList);
            }
            else
            {
                target[fullKey] = pair.Value?.ToString() ?? string.Empty;
                localizedStringList.Add(new LocalizedString(pair.Key, fullKey));
            }
        }
    }
}