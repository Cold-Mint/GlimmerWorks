namespace GlimmerWorks.Engine.Ecs.Component;


/// <summary>
/// <para>TileLayerType</para>
/// <para>瓦片图层枚举</para>
/// </summary>
[Flags]
public enum TileLayerTypes
{
    Background = 1 << 0,
    BackgroundDecoration = 1 << 1,
    Foreground = 1 << 2
}

public class TileLayer : IComponent
{
    public TileLayerTypes LayerType { get; set; } = TileLayerTypes.Background;
}