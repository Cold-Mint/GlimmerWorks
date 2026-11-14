using GlimmerWorks.engine.Vfs;
using Karambolo.PO;
using Microsoft.Extensions.Localization;

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
    private LocalizedString Get(string name)
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
    private LocalizedString GetFormat(string name, params object[] args)
    {
        if (!_strings.TryGetValue(name, out var value))
        {
            return new LocalizedString(name, string.Format(name, args), true);
        }

        return new LocalizedString(name, string.Format(value, args));
    }

    public void Clear()
    {
        _strings.Clear();
        _localizedStringList.Clear();
    }

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
    /// <para>LoadPoFile</para>
    /// <para>读取po文件</para>
    /// </summary>
    /// <param name="virtualFileSystem"></param>
    /// <param name="poFile"></param>
    /// <returns></returns>
    public bool LoadPoFile(VirtualFileSystem virtualFileSystem, VirtualPath poFile)
    {
        var parser = new POParser(new POParserSettings());
        using var stream = virtualFileSystem.OpenRead(poFile);
        var result = parser.Parse(stream);
        if (result.Success)
        {
            var catalog = result.Catalog;
            foreach (var poEntry in catalog)
            {
                if (poEntry.Count <= 0)
                {
                    continue;
                }

                var name = poEntry.Key.Id;
                var value = string.IsNullOrEmpty(poEntry[0]) ? poEntry.Key.Id : poEntry[0];
                _localizedStringList.Add(new LocalizedString(name, value));
                _strings.Add(name, value);
            }

            return true;
        }

        return false;
    }
}