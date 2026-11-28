using Arch.Core;
using GlimmerWorks.Engine.Math;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Terrain;

namespace GlimmerWorks.Engine.World;

public class DimensionRuntime
{
    private Dictionary<ChunkVertexVector, Chunk> _chunks = new();

    private readonly TerrainGenerator _terrainGenerator = new();

    public void GenerateTerrain(DimensionResource dimensionResource, TerrainVertexVector terrainVertexVector)
    {
        // _terrainGenerator.Generator(dimensionResource, terrainVertexVector);
    }
}