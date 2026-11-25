using MemoryPack;

namespace GlimmerWorks.Engine.Cache;

[MemoryPackable]
internal sealed partial class CacheManifest
{
    public int GameCode { get; set; }

    public Dictionary<string, string> PathToId { get; set; } = new();
}