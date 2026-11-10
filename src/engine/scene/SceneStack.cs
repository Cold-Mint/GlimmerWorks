namespace GlimmerWorks.Engine.Scene;

/// <summary>
/// SceneStack.
/// 场景栈。
/// </summary>
public class SceneStack
{
    public Action<Scene>? OnPushScene { get; set; }
    private readonly Stack<Scene> _stack = new();

    public Scene? Current() => _stack.Count == 0 ? null : _stack.Peek();

    public void PushScene(Scene scene)
    {
        if (_stack.Count > 0)
        {
            _stack.Peek().OnPauseScene();
        }

        _stack.Push(scene);
        OnPushScene?.Invoke(scene);
        scene.OnResumeScene();
    }

    public int Count => _stack.Count;

    public void PopScene()
    {
        if (_stack.Count <= 0)
        {
            return;
        }

        _stack.Peek().OnPauseScene();
        _stack.Pop();
        if (_stack.Count > 0)
        {
            _stack.Peek().OnResumeScene();
        }
    }
}