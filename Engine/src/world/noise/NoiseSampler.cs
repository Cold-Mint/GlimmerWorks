using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.engine;

namespace GlimmerWorks.Engine.World.Noise;

/// <summary>
/// <para>NoiseSampler</para>
/// </summary>
public sealed class NoiseSampler
{
    private readonly FastNoiseLite _noise;

    private NoiseSampler(FastNoiseLite noise)
    {
        _noise = noise;
    }

    /// <summary>
    /// <para>Raw noise in [-1,1].</para>
    /// </summary>
    public float Sample(float x, float y) => _noise.GetNoise(x, y);

    /// <summary>
    /// <para>Normalized noise in [0,1].</para>
    /// </summary>
    public float SampleNormalized(float x, float y) => (Sample(x, y) + 1.0F) * 0.5F;

    /// <summary>
    /// <para>Create a sampler from a noise resource and world seed.</para>
    /// </summary>
    public static NoiseSampler Create(NoiseResource resource, long worldSeed)
    {
        var noise = new FastNoiseLite(ToSeed(worldSeed, resource.SeedOffset));
        noise.SetFrequency(resource.Frequency);
        noise.SetNoiseType(ParseNoiseType(resource.NoiseType));
        noise.SetFractalType(ParseFractalType(resource.FractalType));
        noise.SetFractalOctaves(Math.Max(1, resource.Octaves));
        noise.SetFractalLacunarity(resource.Lacunarity);
        noise.SetFractalGain(resource.Gain);
        noise.SetFractalWeightedStrength(resource.WeightedStrength);
        noise.SetFractalPingPongStrength(resource.PingPongStrength);
        noise.SetCellularDistanceFunction(ParseCellularDistance(resource.CellularDistanceFunction));
        noise.SetCellularReturnType(ParseCellularReturn(resource.CellularReturnType));
        noise.SetCellularJitter(resource.CellularJitter);
        return new NoiseSampler(noise);
    }

    /// <summary>
    /// <para>Create a deterministic fallback sampler used when a dimension omits a noise resource.</para>
    /// </summary>
    public static NoiseSampler CreateDefault(long worldSeed, int salt, float frequency = 0.01F, int octaves = 3)
    {
        var noise = new FastNoiseLite(ToSeed(worldSeed, salt));
        noise.SetFrequency(frequency);
        noise.SetNoiseType(FastNoiseLite.NoiseType.OpenSimplex2);
        noise.SetFractalType(FastNoiseLite.FractalType.FBm);
        noise.SetFractalOctaves(Math.Max(1, octaves));
        return new NoiseSampler(noise);
    }

    private static int ToSeed(long worldSeed, int offset)
    {
        var mixed = unchecked((ulong)worldSeed * 6364136223846793005UL + (ulong)(uint)offset);
        return (int)(mixed ^ (mixed >> 32));
    }

    private static FastNoiseLite.NoiseType ParseNoiseType(string value) => value.ToLowerInvariant() switch
    {
        "1" or "open_simplex2s" or "open_simplex_2s" => FastNoiseLite.NoiseType.OpenSimplex2S,
        "2" or "cellular" => FastNoiseLite.NoiseType.Cellular,
        "3" or "perlin" => FastNoiseLite.NoiseType.Perlin,
        "4" or "value_cubic" => FastNoiseLite.NoiseType.ValueCubic,
        "5" or "value" => FastNoiseLite.NoiseType.Value,
        _ => FastNoiseLite.NoiseType.OpenSimplex2
    };

    private static FastNoiseLite.FractalType ParseFractalType(string value) => value.ToLowerInvariant() switch
    {
        "1" or "fbm" => FastNoiseLite.FractalType.FBm,
        "2" or "ridged" => FastNoiseLite.FractalType.Ridged,
        "3" or "ping_pong" => FastNoiseLite.FractalType.PingPong,
        "4" or "domain_warp_progressive" => FastNoiseLite.FractalType.DomainWarpProgressive,
        "5" or "domain_warp_independent" => FastNoiseLite.FractalType.DomainWarpIndependent,
        "0" or "none" => FastNoiseLite.FractalType.None,
        _ => FastNoiseLite.FractalType.None
    };

    private static FastNoiseLite.CellularDistanceFunction ParseCellularDistance(string value) => value
        .ToLowerInvariant() switch
    {
        "0" or "euclidean" => FastNoiseLite.CellularDistanceFunction.Euclidean,
        "2" or "manhattan" => FastNoiseLite.CellularDistanceFunction.Manhattan,
        "3" or "hybrid" => FastNoiseLite.CellularDistanceFunction.Hybrid,
        _ => FastNoiseLite.CellularDistanceFunction.EuclideanSq
    };

    private static FastNoiseLite.CellularReturnType ParseCellularReturn(string value) => value.ToLowerInvariant() switch
    {
        "0" or "cell_value" => FastNoiseLite.CellularReturnType.CellValue,
        "1" or "distance" => FastNoiseLite.CellularReturnType.Distance,
        "2" or "distance2" => FastNoiseLite.CellularReturnType.Distance2,
        "3" or "distance2_add" => FastNoiseLite.CellularReturnType.Distance2Add,
        "4" or "distance2_sub" => FastNoiseLite.CellularReturnType.Distance2Sub,
        "5" or "distance2_mul" => FastNoiseLite.CellularReturnType.Distance2Mul,
        "6" or "distance2_div" => FastNoiseLite.CellularReturnType.Distance2Div,
        _ => FastNoiseLite.CellularReturnType.Distance
    };
}
