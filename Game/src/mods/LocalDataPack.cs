using GlimmerWorks.Engine;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Data;
using GlimmerWorks.Engine.Vfs;

namespace GlimmerWorks.Game.Mods;

public class LocalDataPack(VirtualPath packRootPath, PackManifest manifest)
    : DataPack<GameAppContext>(packRootPath, manifest)
{
    private int _stringMatchLevel;
    private readonly List<Resources> _stringResourcesList = new();

    private void RegisterStrings(GameAppContext appContext,
        FileEntry fileEntry, VirtualPath path)
    {
        var candidates = LocalizationManager.GetCandidates(appContext.LocalizationManager.CurrentLanguage);
        var currentFileName = fileEntry.Name?.ToLowerInvariant();
        if (_stringMatchLevel == 0 && currentFileName == "default.strings.toml")
        {
            //Default match.
            //默认匹配。
            var stringFileResources =
                appContext.VirtualFileSystem.ReadToml<StringSetResources>(path,
                    new PackContext
                    {
                        AppContext = appContext,
                        RequesterPackId = Manifest.Id
                    });
            if (stringFileResources == null)
            {
                return;
            }

            _stringMatchLevel = 1;
            _stringResourcesList.AddRange(stringFileResources.StringList);
            _stringResourcesList.AddRange(stringFileResources.StringTagList);
        }

        foreach (var candidate in candidates)
        {
            var targetFileName = (candidate + ".strings.toml").ToLowerInvariant();
            if (currentFileName == targetFileName)
            {
                //Best match.
                //最佳匹配。
                var stringFileResources =
                    appContext.VirtualFileSystem.ReadToml<StringSetResources>(path,
                        new PackContext
                        {
                            AppContext = appContext,
                            RequesterPackId = Manifest.Id
                        });
                if (stringFileResources == null)
                {
                    return;
                }

                _stringMatchLevel = 2;
                _stringResourcesList.Clear();
                _stringResourcesList.AddRange(stringFileResources.StringList);
                _stringResourcesList.AddRange(stringFileResources.StringTagList);
            }
        }
    }

    protected override List<Resources>? RegisterResourceManually(GameAppContext appContext, string resourceType,
        FileEntry fileEntry, Action<PackMessage> onMessage)
    {
        var path = fileEntry.Path;
        if (path == null)
        {
            return null;
        }

        if (resourceType == "strings" && _stringMatchLevel < 2)
        {
            RegisterStrings(appContext, fileEntry, path);
        }

        return null;
    }


    protected override List<Resources> AfterScanFiles()
    {
        return _stringResourcesList;
    }
}