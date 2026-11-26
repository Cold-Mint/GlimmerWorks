using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;

namespace GlimmerWorks.Engine.World.Terrain;

/// <summary>
/// <para>BiomeMatcher</para>
/// </summary>
public sealed class BiomeMatcher
{
    private readonly IReadOnlyList<BiomeResource> _candidates;

    public BiomeMatcher(IEnumerable<BiomeResource> biomes, ResourceRef dimension)
    {
        Dimension = dimension;
        _candidates = biomes.Where(biome => biome.BelongsTo(dimension)).ToList();
    }

    public ResourceRef Dimension { get; }

    /// <summary>
    /// <para>All biomes belonging to this dimension.</para>
    /// </summary>
    public IReadOnlyList<BiomeResource> Candidates => _candidates;

    /// <summary>
    /// <para>Find the best matching biome. Returns null when no candidate belongs to the dimension.</para>
    /// </summary>
    public BiomeResource? Find(in ClimateSample climate)
    {
        BiomeResource? best = null;
        var bestScore = float.PositiveInfinity;

        foreach (var biome in _candidates)
        {
            var score = 0.0F;
            score += Difference(biome.Humidity, climate.Humidity) * biome.StrictnessHumidity;
            score += Difference(biome.Temperature, climate.Temperature) * biome.StrictnessTemperature;
            score += Difference(biome.Weirdness, climate.Weirdness) * biome.StrictnessWeirdness;
            score += Difference(biome.Erosion, climate.Erosion) * biome.StrictnessErosion;
            score += Difference(biome.Elevation, climate.Elevation) * biome.StrictnessElevation;
            score += Difference(biome.SurfaceProximity, climate.SurfaceProximity) *
                     biome.StrictnessSurfaceProximity;

            if (score < bestScore)
            {
                bestScore = score;
                best = biome;
            }
        }

        return best;
    }

    private static float Difference(float target, float actual)
    {
        var difference = target - actual;
        return difference * difference;
    }
}
