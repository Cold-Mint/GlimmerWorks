namespace GlimmerWorks.Engine;

public static class EngineConstant
{
    /// <summary>
    /// <para>GameCode</para>
    /// <para>游戏版本代号</para>
    /// </summary>
    public const long GameCode = 4;
    
    /// <summary>
    /// <para>CacheCode</para>
    /// <para>缓存系统的版本</para>
    /// </summary>
    /// <remarks>
    ///<para>Modifying the version number will cause the old cache to become invalid.</para>
    ///<para>修改版本号会让旧缓存失效。</para>
    /// </remarks>
    public const long CacheCode = 1;

    /// <summary>
    /// <para>How many Ticks are executed per second?</para>
    /// <para>每秒执行多少Tick</para>
    /// </summary>
    public const uint TicksPerSecond = 25;
}