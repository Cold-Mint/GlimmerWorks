using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.World.Save;
using GlimmerWorks.Engine.World.Tiles;
using MemoryPack;

namespace GlimmerWorks.Engine.World.Storage;

/// <summary>
/// <para>IChunkStorage</para>
/// <para>区块与区块实体的二进制存档读写抽象。</para>
/// </summary>
public interface IChunkStorage
{
    ChunkSaveData? Load(string dimensionId, ChunkVertexVector2D position);

    bool Save(string dimensionId, ChunkSaveData data);

    /// <summary>
    /// <para>Load the entity file for a chunk, or null when absent.</para>
    /// </summary>
    ChunkEntityMessage? LoadEntities(string dimensionId, ChunkVertexVector2D position) => null;

    /// <summary>
    /// <para>Write the entity file for a chunk.</para>
    /// </summary>
    bool SaveEntities(string dimensionId, ChunkVertexVector2D position, ChunkEntityMessage message) => true;

    /// <summary>
    /// <para>Delete the entity file for a chunk, e.g. when it contains no entities.</para>
    /// </summary>
    bool DeleteEntities(string dimensionId, ChunkVertexVector2D position) => true;
}

/// <summary>
/// <para>MemoryPackChunkStorage</para>
/// <para>按文档目录树写 chunk_&lt;x&gt;_&lt;y&gt;.bin / entity_&lt;x&gt;_&lt;y&gt;.bin 的 MemoryPack 存储。</para>
/// </summary>
public sealed class MemoryPackChunkStorage(string saveDirectory) : IChunkStorage
{
    public string SaveDirectory { get; } = saveDirectory;

    public ChunkSaveData? Load(string dimensionId, ChunkVertexVector2D position)
    {
        var path = SavePaths.GetChunkPath(SaveDirectory, dimensionId, position);
        if (!File.Exists(path))
        {
            return null;
        }

        try
        {
            var data = MemoryPackSerializer.Deserialize<ChunkMessage>(File.ReadAllBytes(path));
            if (data is null)
            {
                return null;
            }

            // The filename and the message position must agree.
            if (data.Position.X != position.X || data.Position.Y != position.Y)
            {
                LogCat.Warn($"Chunk save position mismatch: file={position.X},{position.Y} " +
                            $"message={data.Position.X},{data.Position.Y} ({path}).");
                return null;
            }

            return SaveMessageMapper.ToChunkSaveData(data);
        }
        catch (Exception exception)
        {
            LogCat.Error($"Failed to read chunk save {path}: {exception.Message}");
            return null;
        }
    }

    public bool Save(string dimensionId, ChunkSaveData data)
    {
        var position = new ChunkVertexVector2D(data.X, data.Y);
        var path = SavePaths.GetChunkPath(SaveDirectory, dimensionId, position);
        return WriteAtomic(path, MemoryPackSerializer.Serialize(SaveMessageMapper.ToChunkMessage(data)));
    }

    public ChunkEntityMessage? LoadEntities(string dimensionId, ChunkVertexVector2D position)
    {
        var path = SavePaths.GetEntityPath(SaveDirectory, dimensionId, position);
        if (!File.Exists(path))
        {
            return null;
        }

        try
        {
            return MemoryPackSerializer.Deserialize<ChunkEntityMessage>(File.ReadAllBytes(path));
        }
        catch (Exception exception)
        {
            LogCat.Error($"Failed to read entity save {path}: {exception.Message}");
            return null;
        }
    }

    public bool SaveEntities(string dimensionId, ChunkVertexVector2D position, ChunkEntityMessage message)
    {
        var path = SavePaths.GetEntityPath(SaveDirectory, dimensionId, position);
        return WriteAtomic(path, MemoryPackSerializer.Serialize(message));
    }

    public bool DeleteEntities(string dimensionId, ChunkVertexVector2D position)
    {
        try
        {
            var path = SavePaths.GetEntityPath(SaveDirectory, dimensionId, position);
            if (File.Exists(path))
            {
                File.Delete(path);
            }

            return true;
        }
        catch (Exception exception)
        {
            LogCat.Error($"Failed to delete entity save: {exception.Message}");
            return false;
        }
    }

    private static bool WriteAtomic(string path, byte[] bytes)
    {
        var temporaryPath = path + ".tmp";
        try
        {
            var directory = Path.GetDirectoryName(path);
            if (!string.IsNullOrEmpty(directory))
            {
                Directory.CreateDirectory(directory);
            }

            using (var stream = File.Create(temporaryPath))
            {
                stream.Write(bytes);
                stream.Flush(true);
            }

            File.Move(temporaryPath, path, true);
            return true;
        }
        catch (Exception exception)
        {
            LogCat.Error($"Failed to write save {path}: {exception.Message}");
            try
            {
                if (File.Exists(temporaryPath))
                {
                    File.Delete(temporaryPath);
                }
            }
            catch
            {
                // Ignore cleanup failures.
            }

            return false;
        }
    }
}

/// <summary>
/// <para>InMemoryChunkStorage</para>
/// <para>内存存档，供测试与临时世界使用。</para>
/// </summary>
public sealed class InMemoryChunkStorage : IChunkStorage
{
    private readonly Dictionary<(string Dimension, int X, int Y), ChunkSaveData> _chunks = [];
    private readonly Dictionary<(string Dimension, int X, int Y), ChunkEntityMessage> _entities = [];
    private readonly object _lock = new();

    public ChunkSaveData? Load(string dimensionId, ChunkVertexVector2D position)
    {
        lock (_lock)
        {
            return _chunks.GetValueOrDefault((dimensionId, position.X, position.Y));
        }
    }

    public bool Save(string dimensionId, ChunkSaveData data)
    {
        lock (_lock)
        {
            _chunks[(dimensionId, data.X, data.Y)] = data;
            return true;
        }
    }

    public ChunkEntityMessage? LoadEntities(string dimensionId, ChunkVertexVector2D position)
    {
        lock (_lock)
        {
            return _entities.GetValueOrDefault((dimensionId, position.X, position.Y));
        }
    }

    public bool SaveEntities(string dimensionId, ChunkVertexVector2D position, ChunkEntityMessage message)
    {
        lock (_lock)
        {
            _entities[(dimensionId, position.X, position.Y)] = message;
            return true;
        }
    }

    public bool DeleteEntities(string dimensionId, ChunkVertexVector2D position)
    {
        lock (_lock)
        {
            _entities.Remove((dimensionId, position.X, position.Y));
            return true;
        }
    }

    public int Count
    {
        get
        {
            lock (_lock)
            {
                return _chunks.Count;
            }
        }
    }
}
