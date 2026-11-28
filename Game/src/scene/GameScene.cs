using GlimmerWorks.Engine.World;
using GlimmerWorks.Game.Ecs;
using GlimmerWorks.Game.Ecs.Components;
using GlimmerWorks.Game.Graphics.Rendering;
using SDL3;

namespace GlimmerWorks.Game.Scene;

public class GameScene(GameAppContext appContext) : BaseScene(appContext)
{
    private readonly WorldContext _worldContext = new();

    public override void OnCreate()
    {
        base.OnCreate();
        _worldContext.EntityManager.CreateEntity(new CameraComponent());
    }

    public override void OnTick(long deltaMs, long globalTick)
    {
        _worldContext.SystemScheduler.NeedActivate(_worldContext);
        foreach (var system in _worldContext.SystemScheduler.GetActivatedSystemList())
        {
            system.OnTick(deltaMs, globalTick);
        }
    }

    protected override void OnDraw(Renderer2D renderer)
    {
        foreach (var system in _worldContext.SystemScheduler.GetActivatedSystemList())
        {
            if (system is not GameSystem gameSystem)
            {
                continue;
            }

            gameSystem.OnDraw(renderer);
        }
    }

    public override bool Input(SDL.Event sdlEvent)
    {
        foreach (var system in _worldContext.SystemScheduler.GetActivatedSystemList())
        {
            if (system is not GameSystem gameSystem)
            {
                continue;
            }

            if (gameSystem.Input(sdlEvent))
            {
                return true;
            }
        }

        return false;
    }
}