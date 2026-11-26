using System.Text;
using GlimmerWorks.Engine.math;

namespace GlimmerWorks.Engine.World.Save;

/// <summary>
/// <para>SavePaths</para>
/// <para>存档目录树与文件名规则。</para>
/// </summary>
public static class SavePaths
{
    /// <summary>
    /// <para>saves/save_&lt;hash&gt; under the runtime directory.</para>
    /// </summary>
    public static string GetSaveDirectory(string runtimeDirectory, string worldName)
    {
        var hash = Hash64(worldName);
        return Path.Combine(runtimeDirectory, "saves", $"save_{hash}");
    }

    public static string GetManifestPath(string saveDirectory) =>
        Path.Combine(saveDirectory, "manifest.bin");

    public static string GetPlayerDirectory(string saveDirectory) =>
        Path.Combine(saveDirectory, "players");

    public static string GetLocalPlayerPath(string saveDirectory) =>
        Path.Combine(GetPlayerDirectory(saveDirectory), "local_player.bin");

    public static string GetDimensionDirectory(string saveDirectory, string dimensionId)
    {
        var split = dimensionId.Split(':', 2);
        var pack = Sanitize(split.Length > 0 ? split[0] : "core");
        var key = Sanitize(split.Length > 1 ? split[1] : dimensionId);
        return Path.Combine(saveDirectory, "dimensions", $"{pack}_{key}");
    }

    public static string GetChunkDirectory(string saveDirectory, string dimensionId) =>
        Path.Combine(GetDimensionDirectory(saveDirectory, dimensionId), "chunks");

    public static string GetEntityDirectory(string saveDirectory, string dimensionId) =>
        Path.Combine(GetDimensionDirectory(saveDirectory, dimensionId), "entities");

    public static string GetChunkPath(string saveDirectory, string dimensionId, ChunkVertexVector2D position) =>
        Path.Combine(GetChunkDirectory(saveDirectory, dimensionId),
            $"chunk_{position.X}_{position.Y}.bin");

    public static string GetEntityPath(string saveDirectory, string dimensionId, ChunkVertexVector2D position) =>
        Path.Combine(GetEntityDirectory(saveDirectory, dimensionId),
            $"entity_{position.X}_{position.Y}.bin");

    public static string GetDimensionManifestPath(string saveDirectory, string dimensionId) =>
        Path.Combine(GetDimensionDirectory(saveDirectory, dimensionId), "dimension_manifest.bin");

    /// <summary>
    /// <para>Find valid save directories. A valid save has both manifest.bin and local_player.bin.</para>
    /// </summary>
    public static List<string> FindSaveDirectories(string runtimeDirectory)
    {
        var savesRoot = Path.Combine(runtimeDirectory, "saves");
        if (!Directory.Exists(savesRoot))
        {
            return [];
        }

        return Directory.EnumerateDirectories(savesRoot)
            .Where(directory =>
                File.Exists(GetManifestPath(directory)) &&
                File.Exists(GetLocalPlayerPath(directory)))
            .ToList();
    }

    public static bool DeleteSaveDirectory(string saveDirectory)
    {
        try
        {
            if (Directory.Exists(saveDirectory))
            {
                Directory.Delete(saveDirectory, true);
            }

            return true;
        }
        catch
        {
            return false;
        }
    }

    public static ulong Hash64(string value)
    {
        const ulong offset = 14695981039346656037UL;
        const ulong prime = 1099511628211UL;
        var hash = offset;
        foreach (var b in Encoding.UTF8.GetBytes(value))
        {
            hash ^= b;
            hash *= prime;
        }

        return hash;
    }

    private static string Sanitize(string value)
    {
        var chars = value.Select(static c =>
            char.IsLetterOrDigit(c) || c is '_' or '-' or '.' ? c : '_').ToArray();
        var result = new string(chars);
        return string.IsNullOrEmpty(result) ? "core" : result;
    }
}
