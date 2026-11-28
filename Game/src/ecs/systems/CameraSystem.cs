using GlimmerWorks.Game.Ecs.Components;
using JetBrains.Annotations;

namespace GlimmerWorks.Game.Ecs.systems;

[UsedImplicitly]
public class CameraSystem : GameSystem
{
    public override IReadOnlyList<Type> DependentComponents { get; } = [typeof(CameraComponent)];
}