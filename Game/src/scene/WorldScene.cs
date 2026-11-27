using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.World;
using GlimmerWorks.Engine.World.Render;
using GlimmerWorks.Engine.World.Streaming;
using GlimmerWorks.Game.Graphics.Rendering;
using GlimmerWorks.Game.Graphics.Uniform;
using SDL3;
using TileResource = GlimmerWorks.Engine.Mods.Resources.TileResource;
using System.Numerics;

namespace GlimmerWorks.Game.Scene;

/// <summary>
/// Debug world preview. Streams chunks around a free camera and paints each tile as a solid rectangle.
/// This scene proves the generation/streaming/render-list pipeline end to end without requiring tile art.
/// </summary>
public sealed class WorldScene : BaseScene
{
    private const float CameraSpeed = 320.0F;

    private readonly World _world;
    private readonly HashSet<SDL.Scancode> _keys = [];
    private readonly ChunkRenderCache _renderCache = new(new ChunkRenderList());
    private WorldVector2D _camera = new();
    private float _zoom = 1.0F;

    public WorldScene(GameAppContext appContext, World world) : base(appContext)
    {
        _world = world;
    }

    protected override Color ClearColor => new(10, 12, 18);

    public override void OnCreate()
    {
        _world.CurrentDimension.Streaming.Asynchronous = false;
        _world.CurrentDimension.Streaming.PreloadRadiusChunks = 2;

        var surface = _world.CurrentDimension.Terrain.Climate.GetSurfaceHeight(0);
        _camera = new WorldVector2D(0, (surface + 6) * (float)WorldConstants.TilePixelSize);
        UpdateStreaming();
        _world.Tick(0);
    }

    public override void OnTick(long deltaMs, long globalTick)
    {
        var seconds = deltaMs / 1000.0F;
        var dx = 0.0F;
        var dy = 0.0F;
        if (_keys.Contains(SDL.Scancode.A) || _keys.Contains(SDL.Scancode.Left))
        {
            dx -= 1;
        }

        if (_keys.Contains(SDL.Scancode.D) || _keys.Contains(SDL.Scancode.Right))
        {
            dx += 1;
        }

        if (_keys.Contains(SDL.Scancode.W) || _keys.Contains(SDL.Scancode.Up))
        {
            dy += 1;
        }

        if (_keys.Contains(SDL.Scancode.S) || _keys.Contains(SDL.Scancode.Down))
        {
            dy -= 1;
        }

        if (dx != 0 || dy != 0)
        {
            var length = MathF.Sqrt(dx * dx + dy * dy);
            _camera = new WorldVector2D(
                _camera.X + dx / length * CameraSpeed * seconds,
                _camera.Y + dy / length * CameraSpeed * seconds);
        }

        var halfWidth = _world.CurrentDimension.Resource.MaxX * (float)WorldConstants.TilePixelSize;
        var halfHeight = _world.CurrentDimension.Resource.MaxY * (float)WorldConstants.TilePixelSize;
        _camera = new WorldVector2D(
            Math.Clamp(_camera.X, -halfWidth, halfWidth),
            Math.Clamp(_camera.Y, 0, halfHeight));

        UpdateStreaming();
        _world.Tick(deltaMs);
    }

    public override void Input(SDL.Event sdlEvent)
    {
        var eventType = (SDL.EventType)sdlEvent.Type;
        if (eventType == SDL.EventType.KeyDown)
        {
            var scancode = sdlEvent.Key.Scancode;
            if (scancode == SDL.Scancode.Escape)
            {
                AppContext.SceneStack.AddTask(new SceneStackTask { Type = StackType.Pop });
                return;
            }

            _keys.Add(scancode);
        }
        else if (eventType == SDL.EventType.KeyUp)
        {
            _keys.Remove(sdlEvent.Key.Scancode);
        }
        else if (eventType == SDL.EventType.MouseWheel)
        {
            _zoom = Math.Clamp(_zoom + sdlEvent.Wheel.Y * 0.15F, 0.25F, 4.0F);
            UpdateStreaming();
        }
    }

    protected override void OnDraw(Renderer2D renderer)
    {
        var (width, height) = GetViewportSize();
        if (width <= 0 || height <= 0)
        {
            return;
        }

        _world.RefreshLighting();
        RuntimeUniformState.CameraPosition = new Vector2(_camera.X, _camera.Y);
        RuntimeUniformState.CameraZoom = _zoom;
        RuntimeUniformState.SeaLevel =
            _world.CurrentDimension.Resource.SeaLevelY * (float)WorldConstants.TilePixelSize;
        var camera = new CameraView(_camera, new WorldVector2D(width, height), _zoom);
        var commands = _renderCache.Get(_world.CurrentDimension.Store, camera.GetWorldRect());

        foreach (var command in commands)
        {
            var bounds = command.WorldBounds;
            var left = (bounds.Left - _camera.X) * _zoom + width * 0.5F;
            var top = height * 0.5F - (bounds.Top - _camera.Y) * _zoom;
            var rect = new RectF(left, top, bounds.Width * _zoom, bounds.Height * _zoom);
            var material = command.Texture.Material is { } materialRef
                ? renderer.LoadMaterial(materialRef)
                : null;
            if (material != null)
            {
                renderer.DrawMaterial(material, rect, ApplyLight(new Color(255, 255, 255), command));
                continue;
            }

            var baseColor = BaseColor(command.Tile.Resource, command.Layer);
            renderer.FillRectangle(rect, ApplyLight(baseColor, command));
        }
    }

    public override void OnDestroy()
    {
        _world.SaveAll();
        _world.UnloadAll();
        _world.Dispose();
        base.OnDestroy();
    }

    private void UpdateStreaming()
    {
        var (width, height) = GetViewportSize();
        if (width <= 0 || height <= 0)
        {
            return;
        }

        _world.SetCamera(new CameraView(_camera, new WorldVector2D(width, height), _zoom));
    }

    private (int Width, int Height) GetViewportSize()
    {
        if (AppContext.Window != IntPtr.Zero &&
            SDL.GetWindowSizeInPixels(AppContext.Window, out var width, out var height) &&
            width > 0 && height > 0)
        {
            return (width, height);
        }

        return (AppContext.GameConfig.Window.Width, AppContext.GameConfig.Window.Height);
    }

    private Color ApplyLight(Color color, TileDrawCommand command)
    {
        var light = _world.Light.GetFinalColor(command.TopLeftCell, command.Layer);
        var luminance = Math.Clamp(0.25F + light.Luminance * 0.9F, 0.25F, 1.25F);
        return new Color(
            Math.Clamp((int)(color.R * luminance), 0, 255),
            Math.Clamp((int)(color.G * luminance), 0, 255),
            Math.Clamp((int)(color.B * luminance), 0, 255),
            color.A);
    }

    private static Color BaseColor(TileResource resource, int layer)
    {
        if (layer == WorldConstants.BackgroundLayer)
        {
            return new Color(38, 42, 52);
        }

        if (layer == WorldConstants.BackgroundDecorationLayer)
        {
            return new Color(52, 60, 48);
        }

        return resource.ResourceId switch
        {
            "dirt" => new Color(112, 78, 50),
            "stone" => new Color(108, 112, 120),
            "bedrock" => new Color(42, 44, 50),
            "water" => new Color(46, 100, 170, 210),
            "trunk" => new Color(96, 64, 38),
            "leaves" => new Color(58, 128, 62),
            "torch" => new Color(240, 190, 90),
            _ => new Color(190, 80, 190)
        };
    }
}
