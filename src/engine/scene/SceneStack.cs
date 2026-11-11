using System.Collections.Concurrent;

namespace GlimmerWorks.Engine.Scene;

public enum StackType
{
    None,
    Push,
    Pop,
    Replace
}

public class SceneStackTask
{
    public Scene? Scene { get; set; }
    public StackType Type { get; set; }
}

/// <summary>
/// SceneStack.
/// 场景栈。
/// </summary>
public class SceneStack
{
    private readonly Stack<Scene> _stack = new();

    public Scene? Current() => _stack.Count == 0 ? null : _stack.Peek();
    private ConcurrentQueue<SceneStackTask> _tasks = new();

    public int Count => _stack.Count;

    /// <summary>
    /// <para>IsTopScene</para>
    /// <para>某个场景是否在栈顶</para>
    /// </summary>
    /// <param name="scene"></param>
    /// <returns></returns>
    public bool IsTopScene(Scene scene)
    {
        if (_stack.Count <= 0)
        {
            return false;
        }

        return _stack.Peek() == scene;
    }

    /// <summary>
    /// <para>ExecuteTaskQueue</para>
    /// <para>执行任务队列</para>
    /// </summary>
    public void ExecuteTaskQueue()
    {
        while (_tasks.TryDequeue(out var sceneStackTask))
        {
            var scene = sceneStackTask.Scene;
            switch (sceneStackTask.Type)
            {
                case StackType.Pop:
                    if (_stack.Count <= 0)
                    {
                        continue;
                    }

                    var top = _stack.Peek();
                    top.OnPauseScene();
                    top.OnDestroy();
                    _stack.Pop();
                    if (_stack.Count > 0)
                    {
                        _stack.Peek().OnResumeScene();
                    }

                    break;
                case StackType.Push:
                    if (scene == null)
                    {
                        continue;
                    }

                    if (_stack.Count > 0)
                    {
                        _stack.Peek().OnPauseScene();
                    }

                    scene.OnCreate();
                    _stack.Push(scene);
                    scene.OnResumeScene();
                    break;
                case StackType.Replace:
                    if (scene == null)
                    {
                        continue;
                    }

                    if (_stack.Count <= 0)
                    {
                        continue;
                    }

                    var top1 = _stack.Peek();
                    top1.OnPauseScene();
                    top1.OnDestroy();
                    _stack.Pop();

                    scene.OnCreate();
                    _stack.Push(scene);
                    scene.OnResumeScene();
                    break;
            }
        }
    }

    public void AddTask(SceneStackTask sceneStackTask)
    {
        _tasks.Enqueue(sceneStackTask);
    }
}