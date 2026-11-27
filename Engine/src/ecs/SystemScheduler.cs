using System.Diagnostics;
using GlimmerWorks.Engine.Log;

namespace GlimmerWorks.Engine.Ecs;

/// <summary>
/// <para>SystemScheduler</para>
/// <para>按注册顺序每帧调用系统，并隔离单个系统的异常。</para>
/// </summary>
/// <remarks>
/// <para>顺序即依赖：时间推进在生长之前，生长在流式加载之前。</para>
/// <para>顺序由注册顺序决定。异常按系统计数并限流记录日志，避免 40 TPS 下刷屏。</para>
/// </remarks>
public sealed class SystemScheduler
{
    private const int FailureLogInterval = 240;

    private readonly List<ISystem> _systems = [];
    private readonly Dictionary<ISystem, SystemState> _states = new(ReferenceEqualityComparer.Instance);

    /// <summary>
    /// <para>Registered systems in execution order.</para>
    /// </summary>
    public IReadOnlyList<ISystem> Systems => _systems;

    public int Count => _systems.Count;

    /// <summary>
    /// <para>Wall-clock milliseconds spent in the last <see cref="Update"/> call.</para>
    /// </summary>
    public double LastUpdateMs { get; private set; }

    /// <summary>
    /// <para>The system that took the most time during the last update, if any.</para>
    /// </summary>
    public ISystem? LastSlowestSystem { get; private set; }

    public void Add(ISystem system)
    {
        ArgumentNullException.ThrowIfNull(system);
        if (_states.ContainsKey(system))
        {
            throw new InvalidOperationException($"System {system.GetType().Name} is already registered.");
        }

        _systems.Add(system);
        _states[system] = new SystemState();
    }

    public void AddRange(params ISystem[] systems)
    {
        foreach (var system in systems)
        {
            Add(system);
        }
    }

    public bool Remove(ISystem system)
    {
        if (!_states.Remove(system))
        {
            return false;
        }

        return _systems.Remove(system);
    }

    public void Clear()
    {
        _systems.Clear();
        _states.Clear();
    }

    /// <summary>
    /// <para>Run every registered system once.</para>
    /// </summary>
    public void Update(in SystemContext context)
    {
        var updateStart = Stopwatch.GetTimestamp();
        var slowestMs = -1.0;

        foreach (var system in _systems)
        {
            var state = _states[system];
            var start = Stopwatch.GetTimestamp();
            try
            {
                system.Update(in context);
            }
            catch (Exception exception)
            {
                state.Failures++;
                if (state.Failures == 1 || state.Failures % FailureLogInterval == 0)
                {
                    LogCat.Error(
                        $"System {system.GetType().Name} failed ({state.Failures} time(s)): " +
                        $"{exception.GetType().Name}: {exception.Message}");
                }
            }

            var elapsedMs = Stopwatch.GetElapsedTime(start).TotalMilliseconds;
            state.LastMs = elapsedMs;
            state.TotalMs += elapsedMs;
            state.Updates++;
            if (elapsedMs > slowestMs)
            {
                slowestMs = elapsedMs;
            }
        }

        LastUpdateMs = Stopwatch.GetElapsedTime(updateStart).TotalMilliseconds;
        LastSlowestSystem = FindSlowest();
    }

    /// <summary>
    /// <para>Snapshot of per-system timing and failure counters, for debug overlays.</para>
    /// </summary>
    public IReadOnlyList<SystemMetric> GetMetrics()
    {
        return _systems.Select(system =>
        {
            var state = _states[system];
            return new SystemMetric(system.GetType(), state.LastMs, state.TotalMs, state.Updates, state.Failures);
        }).ToList();
    }

    private ISystem? FindSlowest()
    {
        ISystem? slowest = null;
        var slowestMs = -1.0;
        foreach (var system in _systems)
        {
            var lastMs = _states[system].LastMs;
            if (lastMs > slowestMs)
            {
                slowestMs = lastMs;
                slowest = system;
            }
        }

        return slowest;
    }

    private sealed class SystemState
    {
        public double LastMs;
        public double TotalMs;
        public long Updates;
        public long Failures;
    }
}

/// <summary>
/// <para>SystemMetric</para>
/// </summary>
public readonly record struct SystemMetric(
    Type SystemType,
    double LastMs,
    double TotalMs,
    long Updates,
    long Failures)
{
    public double AverageMs => Updates <= 0 ? 0 : TotalMs / Updates;
}
