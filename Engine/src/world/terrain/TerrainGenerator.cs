using GlimmerWorks.Engine.Math;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Noise;

namespace GlimmerWorks.Engine.World.Terrain;

public class TerrainGenerator
{
    /// <summary>
    /// <para>Generate one terrain set from the dimension's elevation noise.</para>
    /// <para>根据维度的高度噪声生成一个地形集合。</para>
    /// </summary>
    /// <param name="appContext">Application context. 应用上下文。</param>
    /// <param name="dimensionResource">Dimension definition. 维度定义。</param>
    /// <param name="terrainVertexVector">Bottom-left vertex of the terrain set, in tiles. 地形集合左下角（瓦片坐标）。</param>
    /// <param name="worldSeed">World seed. 世界种子。</param>
    public TerrainSet Generator(
        AppContext appContext,
        DimensionResource dimensionResource,
        TerrainVertexVector terrainVertexVector,
        long worldSeed)
    {
        var noiseResource = appContext.DataRepository.Get<NoiseResource>(dimensionResource.ElevationNoise);
        var elevation = NoiseSampler.Create(noiseResource, worldSeed);
        var terrainSet = new TerrainSet();

        for (var localX = 0; localX < EngineConstant.TerrainSetEdgeTiles; localX++)
        {
            var worldX = terrainVertexVector.X + localX;
            var normalizedHeight = elevation.SampleNormalized(worldX, 0);
            var surfaceY = dimensionResource.MinContinentalY +
                           (int)((dimensionResource.MaxContinentalY - dimensionResource.MinContinentalY) *
                                 normalizedHeight);

            for (var localY = 0; localY < EngineConstant.TerrainSetEdgeTiles; localY++)
            {
                var worldY = terrainVertexVector.Y + localY;
                var terrainType = worldY <= dimensionResource.MinY
                    ? TerrainType.BedRock
                    : worldY <= surfaceY
                        ? TerrainType.Solid
                        : TerrainType.Air;

                terrainSet.SetTerrainTile(
                    new TerrainRelativeVector((uint)localX, (uint)localY),
                    new TerrainTile { TerrainType = terrainType });
            }
        }

        return terrainSet;
    }

    /// <summary>
    /// <para>Prototype overload with seed 0. Prefer the overload that takes a world seed.</para>
    /// <para>使用种子 0 的原型重载。正式代码请使用带世界种子的重载。</para>
    /// </summary>
    public TerrainSet Generator(
        AppContext appContext,
        DimensionResource dimensionResource,
        TerrainVertexVector terrainVertexVector) =>
        Generator(appContext, dimensionResource, terrainVertexVector, 0);
}
