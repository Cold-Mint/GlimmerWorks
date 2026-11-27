using MemoryPack;

namespace GlimmerWorks.Engine.Cache;

[MemoryPackable]
internal sealed partial class CacheManifest
{
    public long CacheCode { get; set; }

    public Dictionary<string, string> PathToId { get; } = new();
}