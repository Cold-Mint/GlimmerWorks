using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Noise;

namespace GlimmerWorks.Engine.World.Terrain;

/// <summary>
/// <para>DimensionNoiseSet</para>
/// </summary>
public sealed class DimensionNoiseSet(
    NoiseSampler elevation,
    NoiseSampler humidity,
    NoiseSampler temperature,
    NoiseSampler weirdness,
    NoiseSampler erosion)
{
    public NoiseSampler Elevation { get; } = elevation;

    public NoiseSampler Humidity { get; } = humidity;

    public NoiseSampler Temperature { get; } = temperature;

    public NoiseSampler Weirdness { get; } = weirdness;

    public NoiseSampler Erosion { get; } = erosion;
}

/// <summary>
/// <para>ClimateSampler</para>
/// </summary>
public sealed class ClimateSampler(DimensionResource dimension, DimensionNoiseSet noise)
{
    /// <summary>
    /// <para>Hard-coded weirdness coordinate scale.</para>
    /// </summary>
    public const float WeirdnessScale = 1.0F / 3500.0F;

    public DimensionResource Dimension { get; } = dimension;

    public DimensionNoiseSet Noise { get; } = noise;

    /// <summary>
    /// <para>Surface height for a column. The height is sampled once per x and reused by the whole column.</para>
    /// </summary>
    public int GetSurfaceHeight(int worldX)
    {
        var n = (Noise.Elevation.Sample(worldX, 0) + 1.0F) * 0.5F;
        var span = Dimension.MaxContinentalY - Dimension.MinContinentalY;
        return Dimension.MinContinentalY + (int)(span * n);
    }

    /// <summary>
    /// <para>Sample all six climate dimensions for a cell.</para>
    /// </summary>
    public ClimateSample Sample(int worldX, int worldY, int surfaceHeight)
    {
        var humidity = (Noise.Humidity.Sample(worldX, worldY) + 1.0F) * 0.5F;

        var elevation = GetElevation(worldY);
        var temperature = (Noise.Temperature.Sample(worldX, worldY) + 1.0F) * 0.5F *
                          MathF.Pow(Math.Max(0.0F, 1.0F - elevation), 1.5F);

        var weirdness = (Noise.Weirdness.Sample(
            worldX * WeirdnessScale,
            worldY * WeirdnessScale) + 1.0F) * 0.5F;
        var erosion = (Noise.Erosion.Sample(worldX, worldY) + 1.0F) * 0.5F;

        var height = Dimension.MaxY - Dimension.MinY;
        var surfaceNormalized = height <= 0 ? 0.0F : (surfaceHeight - Dimension.MinY) / (float)height;
        var currentNormalized = height <= 0 ? 0.0F : (worldY - Dimension.MinY) / (float)height;
        var surfaceProximity = Math.Clamp(
            0.5F + (currentNormalized - surfaceNormalized) * 0.5F,
            0.0F, 1.0F);

        return new ClimateSample(
            Math.Clamp(humidity, 0.0F, 1.0F),
            Math.Clamp(temperature, 0.0F, 1.0F),
            Math.Clamp(weirdness, 0.0F, 1.0F),
            Math.Clamp(erosion, 0.0F, 1.0F),
            Math.Clamp(elevation, 0.0F, 1.0F),
            surfaceProximity,
            surfaceHeight);
    }

    /// <summary>
    /// <para>Normalized elevation of a Y coordinate.</para>
    /// </summary>
    public float GetElevation(int worldY)
    {
        var span = Dimension.MaxY - Dimension.MinY;
        return span <= 0 ? 0.0F : (worldY - Dimension.MinY) / (float)span;
    }
}
