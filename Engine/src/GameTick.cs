using System.Diagnostics;

namespace GlimmerWorks.Engine;

public class GameTick
{
    private readonly Thread _tickThread;
    private volatile bool _running;
    private readonly double _targetFrameMs;
    public Action<long, long>? OnTick { get; set; }

    public GameTick()
    {
        _targetFrameMs = 1000.0 / EngineConstant.TicksPerSecond;
        _tickThread = new Thread(TickLoop)
        {
            IsBackground = true,
            Name = "GameTickThread"
        };
    }

    public void Start()
    {
        _running = true;
        _tickThread.Start();
    }

    public void Stop()
    {
        _running = false;
        _tickThread.Join();
    }

    private void TickLoop()
    {
        var stopwatch = Stopwatch.StartNew();
        var lastMs = stopwatch.ElapsedMilliseconds;
        long globalTick = 0;
        while (_running)
        {
            globalTick++;
            var nowMs = stopwatch.ElapsedMilliseconds;
            //The actual interval from the beginning of the previous frame to the beginning of this frame.
            //上一帧开始到这一帧开始的实际间隔。
            var deltaMs = nowMs - lastMs;
            lastMs = nowMs;
            OnTick?.Invoke(deltaMs, globalTick);
            //"computeMs" represents the duration of time that "OnTick" spent on the calculation.
            //computeMs是OnTick花费计算了多长时间。
            var computeMs = stopwatch.ElapsedMilliseconds - nowMs;
            var sleepMs = _targetFrameMs - computeMs;
            if (sleepMs <= 0)
            {
                //Frame timeout: Do not wait. Proceed directly to the next frame.
                //帧超时：不等待。直接下一帧。
                continue;
            }

            if (sleepMs < 1)
            {
                //If the next frame is coming in less than 1 millisecond, then spin and wait here until the time is up.
                //如果还有不到1毫秒就下一帧，那么在这里自旋等待。直到到时间。
                var until = stopwatch.ElapsedTicks + (long)(sleepMs / 1000.0 * Stopwatch.Frequency);
                while (stopwatch.ElapsedTicks < until)
                {
                    Thread.SpinWait(10);
                }
            }
            else
            {
                Thread.Sleep((int)sleepMs);
            }
        }
    }
}