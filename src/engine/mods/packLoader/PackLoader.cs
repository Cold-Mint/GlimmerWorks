using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine.Mods.PackLoader;

/// <summary>
/// <para>PackLoader</para>
/// <para>包加载器</para>
/// </summary>
/// <typeparam name="T"></typeparam>
public abstract class PackLoader<T> where T : BasePack
{
    public async Task<int> Scan(AppContext appContext, VirtualPath virtualPath, Action<PackMessage> onMessage)
    {
        return await Task.Run(() =>
        {
            var fileEntryList = appContext.VirtualFileSystem.List(virtualPath);
            if (fileEntryList == null)
            {
                return 0;
            }

            var enabledSet = GetEnabledSet(appContext.GameConfig.Mod);
            List<T> packs = [];
            foreach (var fileEntry in fileEntryList)
            {
                if (!fileEntry.IsDirectory)
                {
                    continue;
                }

                var manifestPath = fileEntry.Path.Combine("manifest.toml");
                if (!appContext.VirtualFileSystem.FileExists(manifestPath))
                {
                    continue;
                }

                var packManifest = appContext.VirtualFileSystem.ReadToml<PackManifest>(manifestPath);
                if (packManifest == null)
                {
                    continue;
                }

                if (!enabledSet.Contains(packManifest.Id))
                {
                    continue;
                }

                var pack = CreatePack(fileEntry.Path, packManifest);
                packs.Add(pack);
                pack.LoadAllTemplate(appContext, onMessage);
            }

            foreach (var basePack in packs)
            {
                basePack.RegisterResourceType(basePack.ResourceTypeDictionary);
                basePack.Load(appContext, onMessage);
            }
            return packs.Count;
        });
    }

    protected abstract HashSet<string> GetEnabledSet(ModConfig modConfig);

    protected abstract T CreatePack(VirtualPath packRootPath, PackManifest manifest);
}