using GlimmerWorks.Engine.Mods;

namespace GlimmerWorks.Engine.World.Tiles;

/// <summary>
/// <para>ResourceFingerprint</para>
/// </summary>
public static class ResourceFingerprint
{
    /// <summary>
    /// <para>Compute a stable FNV-1a fingerprint for a resource reference.</para>
    /// </summary>
    public static ulong Of(ResourceRef? resourceRef)
    {
        if (resourceRef is null)
        {
            return 0;
        }

        const ulong offset = 14695981039346656037UL;
        var hash = offset;
        hash = Append(hash, resourceRef.TargetPackId);
        hash = Append(hash, resourceRef.ResourceType);
        hash = Append(hash, resourceRef.ResourceId);
        return hash;
    }

    private static ulong Append(ulong hash, string value)
    {
        foreach (var c in value)
        {
            hash ^= c;
            hash *= 1099511628211UL;
        }

        hash ^= 0xFF;
        hash *= 1099511628211UL;
        return hash;
    }
}
