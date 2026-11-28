using GlimmerWorks.Engine.Mods;

namespace GlimmerWorks.Engine.World.Terrain;

public struct TerrainTile()
{
    public TerrainType TerrainType { get; set; } = TerrainType.None;
    public ResourceRef? BiomeResourceRef { get; set; } = null;
}