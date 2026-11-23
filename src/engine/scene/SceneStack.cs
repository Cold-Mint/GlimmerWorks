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
    public Scene? Scene { get; init; }
    public StackType Type { get; init; }

    public int RequestCode { get; init; }
}

/// <summary>
/// SceneStack.
/// 场景栈。
/// </summary>
public class SceneStack
{
    private readonly Stack<Scene> _stack = new();

    public Scene? Current() => _stack.Count == 0 ? null : _stack.Peek();
    private readonly ConcurrentQueue<SceneStackTask> _tasks = new();

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
    /// <para>ExecutePop</para>
    /// <para>执行Pop任务</para>
    /// </summary>
    private void ExecutePop()
    {
        if (_stack.Count <= 0)
        {
            return;
        }

        var top = _stack.Peek();
        var result = top.Result();
        var requestCode = top.RequestCode;
        top.OnPauseScene();
        top.OnDestroy();
        _stack.Pop();
        if (_stack.Count > 0)
        {
            var newTop = _stack.Peek();
            if (result != null)
            {
                newTop.OnResult(requestCode, result);
            }

            newTop.OnResumeScene();
        }
    }

    /// <summary>
    /// <para>ExecuteReplace</para>
    /// <para>执行替换</para>
    /// </summary>
    /// <param name="sceneStackTask">
    ///<para>sceneStackTask</para>
    ///<para>场景栈任务</para>
    /// </param>
    private void ExecuteReplace(SceneStackTask sceneStackTask)
    {
        var scene = sceneStackTask.Scene;
        if (scene == null)
        {
            return;
        }

        if (_stack.Count <= 0)
        {
            return;
        }

        var top = _stack.Peek();
        top.OnPauseScene();
        top.OnDestroy();
        _stack.Pop();

        scene.RequestCode = sceneStackTask.RequestCode;
        scene.OnCreate();
        _stack.Push(scene);
        scene.OnResumeScene();
    }

    /// <summary>
    /// <para>ExecutePush</para>
    /// <para>执行Push</para>
    /// </summary>
    /// <param name="sceneStackTask">
    ///<para>sceneStackTask</para>
    ///<para>场景栈任务</para>
    /// </param>
    private void ExecutePush(SceneStackTask sceneStackTask)
    {
        var scene = sceneStackTask.Scene;
        if (scene == null)
        {
            return;
        }

        if (_stack.Count > 0)
        {
            _stack.Peek().OnPauseScene();
        }

        scene.RequestCode = sceneStackTask.RequestCode;
        scene.OnCreate();
        _stack.Push(scene);
        scene.OnResumeScene();
    }

    /// <summary>
    /// <para>ExecuteTaskQueue</para>
    /// <para>执行任务队列</para>
    /// </summary>
    public void ExecuteTaskQueue()
    {
        while (_tasks.TryDequeue(out var sceneStackTask))
        {
            switch (sceneStackTask.Type)
            {
                case StackType.Pop:
                    ExecutePop();
                    break;
                case StackType.Push:
                    ExecutePush(sceneStackTask);
                    break;
                case StackType.Replace:
                    ExecuteReplace(sceneStackTask);
                    break;
                case StackType.None:
                default:
                    break;
            }
        }
    }

    public void AddTask(SceneStackTask sceneStackTask)
    {
        _tasks.Enqueue(sceneStackTask);
    }
}