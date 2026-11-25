using MemoryPack;

namespace GlimmerWorks.Engine.Cache;

[MemoryPackable]
public sealed partial class CacheEntry
{
    public DateTime LastWriteTimeUtc { set; get; }
    public long? Size { set; get; }

    /// <summary>
    /// <para>BLAKE3 hash of the cached file content (32 bytes).</para>
    /// <para>被缓存文件内容的 BLAKE3 哈希（32 字节）。</para>
    /// </summary>
    public byte[] Blake3 { set; get; } = [];

    public string AssetsPackId { set; get; } = string.Empty;

    public byte[] Data { set; get; } = [];
}