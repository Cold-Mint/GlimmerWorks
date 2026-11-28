using GlimmerWorks.Engine.World.Ecs;
using GlimmerWorks.Game.Graphics.Rendering;
using SDL3;

namespace GlimmerWorks.Game.Ecs;

public abstract class GameSystem : ISystem
{
    public virtual bool Input(SDL.Event sdlEvent)
    {
        return false;
    }

    public virtual IReadOnlyList<Type> DependentComponents { get; } = [];

    public void OnDraw(Renderer2D renderer)
    {
        
    }

    public virtual void OnActivateStatusChanged(bool activate)
    {
    }

    public virtual void OnTick(long deltaMs, long globalTick)
    {
    }
}