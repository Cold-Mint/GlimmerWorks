using GlimmerWorks.Engine.Math;
using GlimmerWorks.Engine.World.Ecs;

namespace GlimmerWorks.Game.Ecs.Components;

public struct CameraComponent : IComponent
{
    /// <summary>
    /// <para>Size</para>
    /// <para>相机尺寸</para>
    /// </summary>
    public Size Size { get; set; }

    /// <summary>
    /// <para>Zoom</para>
    /// <para>相机缩放</para>
    /// </summary>
    public float Zoom { get; set; } = 1.0F;

    public CameraComponent()
    {
        Size = default;
        Zoom = 1.0F;
    }
}